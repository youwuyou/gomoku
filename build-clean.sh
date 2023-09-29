#!/bin/bash

# Function to clean the build artifacts
clean_build() {
    # Construct the path to the build directory
    dir_path="$1/cmake-build-debug"
    
    # Remove the build directory if it exists
    if [ -d "$dir_path" ]; then
        rm -r "$dir_path"
        echo "Removed $dir_path"
    else
        echo "$dir_path does not exist"
    fi
}

# Run the clean commands
clean_build "sockpp"
clean_build "."

