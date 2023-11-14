#!/bin/bash

# Ensure ex1 exists
if [ ! -f "ex1" ]; then
    echo "ex1 does not exist. Compile it first."
    exit 1
fi

# Task 1: Check inode, block size, and permissions of ex1
echo "Checking inode, block size, and permissions of ex1"
stat ex1

# Task 2: Copy ex1 to ex2 and check the number of links and inode numbers
echo "Copying ex1 to ex2 and checking their inode numbers"
cp ex1 ex2
echo "Inode and link count for ex1:"
stat -c "%i - %h" ex1
echo "Inode and link count for ex2:"
stat -c "%i - %h" ex2

# Explanation as comments (answers to the questions)
# When copying ex1 to ex2, a new inode is allocated for ex2. They do not share the same inode.
# The link count for both files is 1, indicating each is a separate file.

# Task 3: Identify files with 3 links in /etc
echo "Identifying files with 3 links in /etc"
stat -c "%h - %n" /etc/* | grep "^3 "

# Explanation as comments (answers to the questions)
# The number 3 in the link count represents the number of hard links to the inode.
# In UNIX-like systems, directories typically have at least 2 links: one for the directory itself and one for its entry in the parent directory.
# If the directory contains subdirectories, each subdirectory has a link back to its parent, increasing the link count.
