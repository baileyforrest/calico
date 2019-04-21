#!/bin/bash
SCRIPT_DIR=$(dirname "$0")

clang-format -i -style=Google $($SCRIPT_DIR/all_files.sh)
