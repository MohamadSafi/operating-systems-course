#!/bin/bash

# Task 1: Delete ./tmp folder if it exists and create a symbolic link tmp1 for the folder ./tmp
[ -d "./tmp" ] && rm -r ./tmp
ln -s ./tmp tmp1

# Task 2: Run ls -li and observe
echo "Running ls -li before creating ./tmp"
ls -li

# Task 3: Create the folder ./tmp
mkdir ./tmp

# Task 4: Run ls -li again and compare
echo "Running ls -li after creating ./tmp"
ls -li

# Explanation as comments (answers to the questions)
# Before creating ./tmp, the link tmp1 appears as a broken link. After creating ./tmp, tmp1 points to an actual directory.

# Task 5: Create a file ex1.txt in ./tmp and check ./tmp1
./gen.sh 10 ./tmp/ex1.txt
echo "Checking contents of ./tmp1 after adding ex1.txt to ./tmp"
ls -l ./tmp1

# Task 6: Create another symbolic link tmp1/tmp2 to ./tmp
mkdir -p ./tmp1/tmp2
ln -s $(realpath ./tmp) ./tmp1/tmp2

# Task 7: Create another file ex1.txt in ./tmp1/tmp2
./gen.sh 10 ./tmp1/tmp2/ex1.txt

# Task 8: Check the content of the sub-folders and try to access nested tmp2 folders
echo "Checking contents of ./tmp1/tmp2"
ls -l ./tmp1/tmp2
echo "Trying to access nested tmp2 folders"
ls -l ./tmp1/tmp2/tmp2/tmp2

# Explanation as comments (answers to the questions)
# The nested tmp2 folders create a loop, as the symbolic link points back to its own parent directory. This leads to an infinitely recursive directory structure.

# Task 9: Delete the folder ./tmp and check the symbolic links again
rm -r ./tmp
echo "Checking symbolic links after deleting ./tmp"
ls -li tmp1
ls -li tmp1/tmp2

# Task 10: Delete all other symbolic links created
rm -f tmp1
rm -f tmp1/tmp2

echo "All tasks completed."
