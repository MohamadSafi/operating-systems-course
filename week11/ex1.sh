#!/bin/bash

# Compling the C program ex1.c
gcc -o ex1 ex1.c

# Creating a file named lofs.img of size 50 MiB
dd if=/dev/zero of=lofs.img bs=1M count=50

# Setting up a loop device on the created file
sudo losetup -f lofs.img

# Finding the loop device that was assigned
LOOP_DEVICE=$(sudo losetup -f --show lofs.img)

# Creating an ext4 filesystem on the loop device
sudo mkfs.ext4 $LOOP_DEVICE

# Creating lofsdisk to mount our filesystem
mkdir -p ./lofsdisk

# Mounting the filesystem on the newly created directory
sudo mount $LOOP_DEVICE ./lofsdisk

# Adding two files file1 and file2 with my first and last name
echo "Mohammad" > ./lofsdisk/file1
echo "Safi" > ./lofsdisk/file2

# Function to get shared libraries of a binary file
get_libs() {
    ldd $1 | awk 'NF > 2 {print $3}' | grep "^/"
}

# Creating a new directories in lofsdisk to paste the needed libraries in
sudo mkdir -p ./lofsdisk/bin ./lofsdisk/lib ./lofsdisk/lib64

# Copying ld-linux-x86-64.so.2 for x64 based operating system (Needed for bash to work)
sudo cp /lib64/ld-linux-x86-64.so.2 ./lofsdisk/lib64/

# Copying shared libraries of specific binaries to LOFSDISK
for BIN in bash cat echo ls; do
    BIN_PATH=$(which $BIN)
    sudo cp $BIN_PATH ./lofsdisk/bin/
    LIBS=$(get_libs $BIN_PATH)
    for LIB in $LIBS; do
        LIB_DIR=$(dirname $LIB)
        sudo mkdir -p ./lofsdisk$LIB_DIR
        sudo cp $LIB ./lofsdisk$LIB_DIR
    done
done

# Copying the compiled C program to the LOFSDISK
sudo cp ex1 ./lofsdisk

# Changing root and run the C program to get the content of the current Root directory
sudo chroot ./lofsdisk /bin/bash -c "./ex1 > ex1.txt"

# Running the program again without changing root
./ex1 >> ex1.txt


