#!/bin/bash

# Navigate to sockpp directory
cd sockpp/

# Create build directory and navigate into it
mkdir -p cmake-build-debug && cd cmake-build-debug

# Run cmake and make
cmake .. && make

# Navigate back to the root directory
cd ../..

# Create another build directory and navigate into it
mkdir -p cmake-build-debug && cd cmake-build-debug

# Run cmake and make again
cmake .. && make

