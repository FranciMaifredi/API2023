use std::io::Write;

use colored::*;
use lazy_static::lazy_static;
use tokio::sync::OnceCell;

lazy_static! {
    pub static ref PR: OnceCell<Printer> = OnceCell::new();
}

#[macro_export]
macro_rules! both {
    ($($arg:tt)*) => {
        $crate::printer::PR.get().unwrap().short(&format!($($arg)*));
        $crate::printer::PR.get().unwrap().long(&format!($($arg)*));
        $crate::printer::PR.get().unwrap().shortln();
    };
}

#[macro_export]
macro_rules! info {
    ($($arg:tt)*) => {
        $crate::printer::PR.get().unwrap().info(&format!($($arg)*));
    };
}

#[macro_export]
macro_rules! short {
    ($($arg:tt)*) => {
        $crate::printer::PR.get().unwrap().short(&format!($($arg)*));
    };
}

#[macro_export]
macro_rules! shortln {
    ($($arg:tt)*) => {
        $crate::printer::PR.get().unwrap().shortln();
    };
}

#[macro_export]
macro_rules! long {
    ($($arg:tt)*) => {
        $crate::printer::PR.get().unwrap().long(&format!($($arg)*));
    };
}

#[derive(Debug, Clone)]
pub enum Level {
    Short,
    Long,
}

#[derive(Debug, Clone)]
pub struct Printer(Level);

pub fn init_printer(level: Level) {
    PR.set(Printer(level)).unwrap();
}

impl From<u8> for Level {
    fn from(level: u8) -> Self {
        match level {
            0 => Level::Short,
            _ => Level::Long,
        }
    }
}

impl From<u8> for Printer {
    fn from(level: u8) -> Self {
        Printer(level.into())
    }
}

impl Printer {
    pub fn info(&self, msg: &str) {
        let tag = format!("[{}]", "#".bold().blue());
        println!("{}: {}", tag, msg.bold().white());
    }

    pub fn short(&self, msg: &str) {
        match self.0 {
            Level::Short => {
                print!("{}", msg);
                std::io::stdout().flush().unwrap();
            }
            Level::Long => (),
        };
    }

    pub fn shortln(&self) {
        match self.0 {
            Level::Short => println!(),
            Level::Long => (),
        };
    }

    pub fn long(&self, msg: &str) {
        match self.0 {
            Level::Short => (),
            Level::Long => println!("{}", msg),
        };
    }
}
