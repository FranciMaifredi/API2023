alias c := compile

_list:
    just --list

compile:
    #!/usr/bin/env bash
    set -euxo
    gcc -Wall ./src/progettoAPI.c -o progettoAPI

clean:
    #!/usr/bin/env bash
    set -eux
    rm -rf ./progettoAPI ./test-runner
    cd utils/test-runner
    cargo clean
