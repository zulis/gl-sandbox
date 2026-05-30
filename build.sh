#!/usr/bin/env bash
set -euo pipefail

# Build directory relative to the repository root.
BUILD_DIR="build"

# Create build directory if it does not exist.
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure the project with CMake.
# Adjust CMAKE_BUILD_TYPE or other options as needed.
cmake -S .. -B . \
  -DCMAKE_BUILD_TYPE=Release

# Build the default target.
cmake --build .

# Optional: run tests if CTest is available.
if cmake --build . --target help | grep -q "test"; then
  echo "\nRunning tests..."
  ctest --output-on-failure
fi
