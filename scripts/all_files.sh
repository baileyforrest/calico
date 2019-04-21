#!/bin/bash

DIRS=(
  'src'
)

SCRIPT_DIR=$(dirname "$0")
cd $SCRIPT_DIR/..

for dir in $DIRS; do
  find $PWD/$dir -name '*.h' -o -name '*.cc'
done
