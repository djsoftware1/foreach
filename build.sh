#!/usr/bin/env sh
# https://github.com/djsoftware1/foreach
#
# Created 2 Jan 2026 by David Joffe

set -eu

# Simple build helper for foreach
# Requires: cmake, a C++17 compiler

BUILD_DIR=${BUILD_DIR:-build}
BUILD_TYPE=${BUILD_TYPE:-Release}

echo "==> Building foreach"
echo "    build dir : $BUILD_DIR"
echo "    build type: $BUILD_TYPE"

cmake -S . -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE="$BUILD_TYPE"
cmake --build "$BUILD_DIR"
