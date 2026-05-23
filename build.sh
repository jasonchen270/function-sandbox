#!/bin/sh
# Build the app. Discovery is a build concern: every core/*.cpp and every
# overrides/*.cpp is compiled straight into the executable, so each override's
# self-registering static initializer runs at startup. Compiling into the exe
# (rather than a static lib) is what keeps those initializers from being
# stripped, so no -Wl,--whole-archive is needed.
set -e
cd "$(dirname "$0")"
mkdir -p build

overrides=""
for f in overrides/*.cpp; do
    [ -e "$f" ] && overrides="$overrides $f"
done

clang++ -std=c++20 -I. app/main.cpp core/*.cpp $overrides -o build/app
echo "built build/app  (overrides:${overrides:- none})"
