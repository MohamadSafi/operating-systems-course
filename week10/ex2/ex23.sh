#!/bin/bash

# Task 1: Create gen.sh
echo 'for (( i=0; i <$1; i++ )); do echo $RANDOM >> $2; done' > gen.sh
chmod +x gen.sh

# Task 2: Create ex1.txt using gen.sh
./gen.sh 10 ex1.txt

# Task 3: Link ex1.txt to ex11.txt and ex12.txt
ln ex1.txt ex11.txt
ln ex1.txt ex12.txt

# Task 4: Compare the content of all files
echo "Comparing the content of ex1.txt, ex11.txt, and ex12.txt"
diff ex1.txt ex11.txt
diff ex1.txt ex12.txt

# Explanation as comments (answers to the questions)
# There is no difference between the files because hard links point to the same inode and hence the same content.

# Task 5: Check inode numbers and save to output.txt
echo "Checking inode numbers"
stat -c "%i" ex1.txt ex11.txt ex12.txt > output.txt

# Task 6: Check disk usage
echo "Checking disk usage of ex1.txt"
du ex1.txt

# Task 7: Create a hard link ex13.txt and move to /tmp
ln ex1.txt ex13.txt
mv ex13.txt /tmp

# Task 8: Trace all links
echo "Tracing links in current path"
find . -inum $(stat -c "%i" ex1.txt)
echo "Tracing links in root path"
find / -inum $(stat -c "%i" ex1.txt) 2>/dev/null

# Explanation as comments (answers to the questions)
# Searching in the current path will find links in the current directory.
# Searching in the root path (/) will find links throughout the entire file system.

# Task 9: Check the number of hard links of ex1.txt
echo "Checking the number of hard links for ex1.txt"
stat -c "%h" ex1.txt

# Task 10: Remove all links from ex1.txt
echo "Removing all links from ex1.txt"
find . -inum $(stat -c "%i" ex1.txt) -exec rm {} \;

# Explanation as comments (answers to the questions)
# The number of hard links of ex1.txt initially includes ex11.txt, ex12.txt, and the link in /tmp. 
# Removing all links with 'find -exec rm {} \;' deletes all files that share the same inode.
