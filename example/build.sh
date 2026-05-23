#!/bin/sh
# Build the example into build/app.
set -e
cd "$(dirname "$0")"
mkdir -p build
"${CXX:-c++}" -std=c++17 -O2 -I. main.cpp area.cpp -o build/app
echo "built build/app"
