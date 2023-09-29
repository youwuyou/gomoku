#!/bin/bash

build_system=$1 # Get the build system from command line argument

# Function to run the build commands
run_build() {
    # Navigate to the directory
    cd $1
    
    # Create build directory and navigate into it
    mkdir -p cmake-build-debug && cd cmake-build-debug
    
    # Run cmake with the specified build system and build
    cmake -G "${build_system}" .. && cmake --build .
    
    # Navigate back to the root directory
    cd ../..
}

# Check if build system is specified and is either "Unix Makefiles" or "Ninja"
if [ -z "$build_system" ] || { [ "$build_system" != "Unix Makefiles" ] && [ "$build_system" != "Ninja" ]; }; then
    echo "Usage: $0 <build_system>"
    echo "build_system: Unix Makefiles | Ninja"
    exit 1
fi

# Run the build commands
run_build "sockpp"
run_build "."

