#!/bin/bash

# Task 1: Make an empty file ex5.txt
touch ex5.txt

# Task 2: Remove write permission for everybody
echo "Removing write permission for everybody on ex5.txt"
chmod a-w ex5.txt
ls -l ex5.txt

# Task 3: Grant all permissions to owner and others (not group)
echo "Granting all permissions to owner and others (not group) on ex5.txt"
chmod ugo=rwx,g= ex5.txt
ls -l ex5.txt

# Task 4: Make group permissions equal to user permissions
echo "Making group permissions equal to user permissions on ex5.txt"
chmod g=u ex5.txt
ls -l ex5.txt

# Explanation as comments (answers to the questions)
# 660 for ex5.txt means: Read and write permissions for the owner and the group, no permissions for others.
# 775 for ex5.txt means: All permissions for the owner and the group, read and execute permissions for others.
# 777 for ex5.txt means: All permissions for the owner, the group, and others.

# Task 5: Set permissions to 660 and check
chmod 660 ex5.txt
echo "Permissions set to 660:"
ls -l ex5.txt

# Task 6: Set permissions to 775 and check
chmod 775 ex5.txt
echo "Permissions set to 775:"
ls -l ex5.txt

# Task 7: Set permissions to 777 and check
chmod 777 ex5.txt
echo "Permissions set to 777:"
ls -l ex5.txt

echo "All tasks completed."
