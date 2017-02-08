#!/bin/bash

# go to build dir:
BASEDIR=$(dirname "$0")
cd $BASEDIR
if [ ! -d "build" ]; then
    mkdir build
fi
cd build

echo "Building in $(pwd)"
cmake -D CMAKE_C_COMPILER=clang ..
make
