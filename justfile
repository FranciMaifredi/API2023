alias cc := compile-c
alias cr := compile-rust
alias crr := compile-rust-release
alias t := test

_list:
    just --list

compile-c:
    #!/usr/bin/env bash
    set -eux
    gcc -Wall ./src/progettoAPI.c -o progettoAPI

compile-rust:
    #!/usr/bin/env bash
    set -eux
    cd utils/test-runner
    cargo build
    mv target/debug/test-runner ../..

compile-rust-release:
    #!/usr/bin/env bash
    set -eux
    cd utils/test-runner
    cargo build --release
    mv target/release/test-runner ../..

test *ARGS:
    ./test-runner ./progettoAPI ./tests {{ARGS}}

clean:
    #!/usr/bin/env bash
    set -eux
    rm -rf ./progettoAPI ./test-runner
    cd utils/test-runner
    cargo clean
