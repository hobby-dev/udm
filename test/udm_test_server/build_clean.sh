#!/bin/bash
BASEDIR=$(dirname "$0")
cd $BASEDIR
echo "Cleanup build in $(pwd)"
rm -rf build
