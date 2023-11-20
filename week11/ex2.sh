#!/bin/bash

# Check if an input file is provided
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <input file>"
    exit 1
fi

# Compile the create_fs.c program
gcc create_fs.c -o create_fs


# Run the create_fs program to create the file system
./create_fs "$1"


# Compile the ex2.c program
gcc ex2.c -o ex2


# Run the ex2 program with the input file
./ex2 "$1"


echo "File system created and input processed successfully."
