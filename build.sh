#!/bin/sh
# Build the tool into ./function-sandbox.
set -e
cd "$(dirname "$0")"
"${CXX:-c++}" -std=c++17 -O2 function-sandbox.cpp -o function-sandbox
echo "built ./function-sandbox"
