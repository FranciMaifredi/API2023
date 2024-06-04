mod printer;

use std::{collections::HashMap, io::read_to_string, path::PathBuf, process::Stdio};

use anyhow::{bail, Context, Result};
use clap::Parser;
use colored::*;
use lazy_static::lazy_static;
use time::{Duration, Instant};
use tokio::{
    fs::File,
    io::{AsyncReadExt, AsyncWriteExt},
    process::Command,
    sync::Semaphore,
    task::JoinSet,
};
use walkdir::WalkDir;

use crate::printer::init_printer;

lazy_static! {
    static ref SEM: Semaphore = Semaphore::new(10);
}

#[derive(Parser, Debug)]
struct Args {
    /// executable to run the tests against
    #[arg(value_name = "FILE")]
    executable: PathBuf,

    /// test dir containing all txt files
    #[arg(value_name = "DIR")]
    test_dir: PathBuf,

    /// verbose settings
    #[arg(short, long, action = clap::ArgAction::Count)]
    verbose: u8,
}

#[derive(Debug)]
enum TestOutcome {
    Correct,
    Incorrect,
    Looped,
    Crashed,
}

#[derive(Debug)]
struct TestJob {
    executable: PathBuf,
    test_file: PathBuf,
}

#[derive(Debug)]
struct TestResult {
    test_file: PathBuf,
    status: TestOutcome,
    timing: Duration,
}

impl TestJob {
    fn new(executable: PathBuf, test_file: PathBuf) -> Self {
        Self {
            executable,
            test_file,
        }
    }

    async fn execute(self) -> Result<TestResult> {
        let file =
            std::fs::File::open(&self.test_file).context("Cannot open test file for reading")?;
        let input_txt = read_to_string(file).context("Cannot read from test file")?;
        let start = Instant::now();
        let timeout = std::time::Duration::from_secs(5);
        let test_file = self.test_file.clone();
        let timeout_ret = tokio::time::timeout(timeout, async move {
            let mut child = Command::new(self.executable)
                .stdin(Stdio::piped())
                .stdout(Stdio::piped())
                .kill_on_drop(true)
                .spawn()
                .context("Failed to spawn child process")?;
            let mut stdin = child.stdin.take().unwrap();
            // let mut stdout = child.stdout.take().unwrap();
            // let mut output_bytes = Vec::new();
            let handler = tokio::spawn(async move {
                stdin.write_all(input_txt.as_bytes()).await?;
                drop(stdin);
                Ok::<_, anyhow::Error>(())
            });
            // stdout.read_(&mut output_bytes).await?;
            // drop(stdout);
            let output = child.wait_with_output().await?;
            if (handler.await?).is_err() {
                return Ok::<_, anyhow::Error>(TestResult {
                    test_file: self.test_file,
                    status: TestOutcome::Crashed,
                    timing: start.elapsed(),
                });
            }

            // drop early if crashed
            if !output.status.success() {
                return Ok::<_, anyhow::Error>(TestResult {
                    test_file: self.test_file,
                    status: TestOutcome::Crashed,
                    timing: start.elapsed(),
                });
            }

            // drop if incorrect
            let actual_output = String::from_utf8_lossy(&output.stdout);
            let mut correct_output = String::new();
            File::open(self.test_file.with_extension("output.txt"))
                .await
                .context(format!(
                    "Cannot find the correct output file of {}",
                    &self.test_file.display()
                ))?
                .read_to_string(&mut correct_output)
                .await?;
            let outcome = if correct_output != actual_output {
                TestOutcome::Incorrect
            } else {
                TestOutcome::Correct
            };
            Ok(TestResult {
                test_file: self.test_file,
                status: outcome,
                timing: start.elapsed(),
            })
        })
        .await;
        match timeout_ret {
            Ok(Ok(r)) => Ok(r),
            Ok(Err(e)) => bail!(e),
            Err(_) => Ok(TestResult {
                test_file,
                status: TestOutcome::Looped,
                timing: start.elapsed(),
            }),
        }
    }
}

#[tokio::main]
async fn main() -> Result<()> {
    let args = Args::parse();
    init_printer(args.verbose.into());

    // get jobs from test dir
    let jobs = WalkDir::new(args.test_dir)
        .into_iter()
        // collect only txt files
        .filter_map(|entry| {
            let entry = entry.context("failed to read test dir entry").unwrap();
            let is_txt = entry.path().extension()? == "txt";
            let is_output = entry.path().with_extension("").extension().is_some();
            if !is_txt || is_output {
                return None;
            }
            Some(TestJob::new(args.executable.clone(), entry.into_path()))
        })
        .collect::<Vec<_>>();
    info!("Collected {} tasks", jobs.len());

    // tidy them up in dirs
    let mut jobs_by_dir: HashMap<PathBuf, Vec<TestJob>> = HashMap::new();
    for job in jobs.into_iter() {
        if let Some(dir) = job.test_file.parent() {
            jobs_by_dir.entry(dir.to_owned()).or_default();
            jobs_by_dir
                .entry(dir.to_owned())
                .and_modify(|v| v.push(job));
        }
    }
    info!("Split into {} dirs", jobs_by_dir.keys().len());

    // execute all jobs of the same dir in parallel
    let mut results = Vec::with_capacity(jobs_by_dir.len());
    for (dir, jobs) in jobs_by_dir.into_iter() {
        short!("{}: ", dir.display().to_string().italic());
        long!("{}:", dir.display().to_string().italic());
        let mut tasks = JoinSet::new();
        for job in jobs {
            tasks.spawn(async move {
                let _s = SEM.acquire().await.unwrap();
                // info!("doing {}", job.test_file.display());
                match job.execute().await {
                    Ok(r) => {
                        let (short_msg, long_msg) = match r.status {
                            TestOutcome::Correct => (".".green(), "SUCCESS".green()),
                            TestOutcome::Incorrect => ("F".red().bold(), "FAIL".red().bold()),
                            TestOutcome::Crashed => {
                                ("C".yellow().bold(), "CRASHED".yellow().bold())
                            }
                            TestOutcome::Looped => ("L".cyan().bold(), "LOOPED".cyan().bold()),
                        };
                        short!("{}", short_msg);
                        long!("  {} - {}", long_msg, r.test_file.display());
                        Ok(r)
                    }
                    Err(e) => {
                        bail!(e)
                    }
                }
            });
        }
        while let Some(res) = tasks.join_next().await {
            match res.unwrap() {
                Ok(test_result) => results.push(test_result),
                Err(e) => bail!(e),
            };
        }
        shortln!();
    }

    // print statistics
    let mut correct_jobs = Vec::new();
    let mut failed_jobs = Vec::new();
    let mut crashed_jobs = Vec::new();
    let mut looped_jobs = Vec::new();
    for res in results {
        match res.status {
            TestOutcome::Correct => correct_jobs.push(res),
            TestOutcome::Incorrect => failed_jobs.push(res),
            TestOutcome::Looped => looped_jobs.push(res),
            TestOutcome::Crashed => crashed_jobs.push(res),
        };
    }

    both!("");
    info!("Statistics:");
    if !correct_jobs.is_empty() {
        both!("{}: {}", "CORRECT".green(), correct_jobs.len());
    }
    if !failed_jobs.is_empty() {
        both!("{}: {}", "INCORRECT".red().bold(), failed_jobs.len());
        for job in failed_jobs {
            long!("  {}", job.test_file.display().to_string().italic().red());
        }
    }
    if !crashed_jobs.is_empty() {
        both!("{}: {}", "CRASHED".yellow().bold(), crashed_jobs.len());
        for job in crashed_jobs {
            long!(
                "  {}",
                job.test_file.display().to_string().italic().yellow()
            );
        }
    }
    if !looped_jobs.is_empty() {
        both!("{}: {}", "LOOPED".cyan().bold(), looped_jobs.len());
        for job in looped_jobs {
            long!("  {}", job.test_file.display().to_string().italic().cyan());
        }
    }

    Ok(())
}
