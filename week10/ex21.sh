#!/bin/bash

# Task 1: Check file types in /dev and /etc
echo "Checking file types in /dev and /etc"
file /dev/*
file /etc/*

# Task 2: Count the number of directories in /etc
echo "Counting the number of directories in /etc"
ls -l /etc | grep ^d | wc -l

# Task 3: Write, compile, and check file type of a Hello World program
echo "Writing and compiling Hello World program"
echo '#include <stdio.h>' > ex1.c
echo 'int main() { printf("Hello world!\\n"); return 0; }' >> ex1.c

# Check file type before compilation
echo "Checking file type of ex1.c before compilation"
file ex1.c

# Compile the program
gcc ex1.c -o ex1

# Check file type after compilation
echo "Checking file type of ex1 after compilation"
file ex1

# Task 4: Modify the program, recompile, and check file type
echo "Modifying to print non-English characters"
sed -i 's/Hello world!/Привет, мир!/' ex1.c

# Check file type after modification but before recompilation
echo "Checking file type of modified ex1.c"
file ex1.c

# Recompile the program
gcc ex1.c -o ex1

# Check file type after recompilation
echo "Checking file type of recompiled ex1"
file ex1

# Explanation as comments (answers to the questions)
# The file type of ex1.c remains the same (ASCII text) before and after modifying the source code.
# The compiled executable file ex1 is identified as an executable file before and after the modification and recompilation.
