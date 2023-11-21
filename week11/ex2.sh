#!/bin/bash

# To Run: ./ex2.sh disk0 input.txt

# Checking if an input file and the disk name is provided
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <disk name> <input file> "
    exit 1
fi

# Compile the create_fs.c program
gcc create_fs.c -o create_fs


# Run the create_fs program to create the file system
./create_fs "$1"


# Compile the ex2.c program
gcc ex2.c -o ex2


# Run the ex2 program with the input file
./ex2 "$1" "$2"


echo "File system created and input processed successfully."
