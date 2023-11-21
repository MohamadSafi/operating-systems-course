#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FILES 16
#define BLOCK_SIZE 1024
#define MAX_FILENAME 16
#define INODE_SIZE 56
#define SUPERBLOCK_SIZE 1024
#define FREE_BLOCK_LIST_SIZE 128
#define INODE_START (FREE_BLOCK_LIST_SIZE)
#define DATA_BLOCK_START (SUPERBLOCK_SIZE)

struct inode {
    char name[MAX_FILENAME];
    int size;
    int blockPointers[8];
    int used;
};

int disk_fd;

void read_free_block_list(char *free_block_list);
void read_inode(int inode_index, struct inode *node);
void write_free_block_list(const char *free_block_list);
void write_inode(int inode_index, struct inode *node);


void read_free_block_list(char *free_block_list) {
    // Seek to the beginning of the disk file
    lseek(disk_fd, 0, SEEK_SET);

    // Read the free block list into the provided array
    read(disk_fd, free_block_list, FREE_BLOCK_LIST_SIZE);
}

void read_inode(int inode_index, struct inode *node) {
    // Calculate the position of the inode
    off_t pos = INODE_START + inode_index * sizeof(struct inode);

    // Seek to the inode's position in the disk file
    lseek(disk_fd, pos, SEEK_SET);

    // Read the inode into the provided struct
    read(disk_fd, node, sizeof(struct inode));
}

void write_free_block_list(const char *free_block_list) {
    // Seek to the beginning of the disk file
    lseek(disk_fd, 0, SEEK_SET);

    // Write the free block list to the disk
    write(disk_fd, free_block_list, FREE_BLOCK_LIST_SIZE);
}

void write_inode(int inode_index, struct inode *node) {
    // Calculate the position of the inode
    off_t pos = INODE_START + inode_index * sizeof(struct inode);

    // Seek to the inode's position in the disk file
    lseek(disk_fd, pos, SEEK_SET);

    // Write the inode to the disk
    write(disk_fd, node, sizeof(struct inode));
}



int create(char name[MAX_FILENAME], int size) {
    char free_block_list[FREE_BLOCK_LIST_SIZE];
    struct inode node;
    int free_block_count = 0, i, j;

    read_free_block_list(free_block_list);
    for (i = 0; i < FREE_BLOCK_LIST_SIZE; i++) {
        if (free_block_list[i] == 0) free_block_count++;
    }
    if (free_block_count < size) return -1; // Not enough space

    for (i = 0; i < MAX_FILES; i++) {
        read_inode(i, &node);
        if (node.used == 0) break;
    }
    if (i == MAX_FILES) return -1; // No free inode found

    // Initialize inode
    strncpy(node.name, name, MAX_FILENAME);
    node.size = size;
    node.used = 1;

    for (j = 0, free_block_count = 0; j < FREE_BLOCK_LIST_SIZE && free_block_count < size; j++) {
        if (free_block_list[j] == 0) {
            free_block_list[j] = 1; // Mark block as in-use
            node.blockPointers[free_block_count++] = j;
        }
    }

    write_free_block_list(free_block_list);
    write_inode(i, &node);

    return 0; 
}

int delete(char name[MAX_FILENAME]) {
    char free_block_list[FREE_BLOCK_LIST_SIZE];
    struct inode node;
    int i, j;

    read_free_block_list(free_block_list);

    for (i = 0; i < MAX_FILES; i++) {
        read_inode(i, &node);
        if (node.used == 1 && strncmp(node.name, name, MAX_FILENAME) == 0) {
            break; // Found the inode
        }
    }
    if (i == MAX_FILES) return -1; // File not found

    for (j = 0; j < node.size; j++) {
        int blockNum = node.blockPointers[j];
        if (blockNum < FREE_BLOCK_LIST_SIZE) {
            free_block_list[blockNum] = 0; // Mark block as free
        }
    }

    node.used = 0;
    memset(&node.name, 0, MAX_FILENAME);  // Clear the file name
    node.size = 0;
    memset(&node.blockPointers, 0, sizeof(node.blockPointers));  // Clear the block pointers

    write_free_block_list(free_block_list);
    write_inode(i, &node);

    return 0; 
}


int ls(void) {
    struct inode node;
    int i;

    // Iterate over each inode
    for (i = 0; i < MAX_FILES; i++) {
        read_inode(i, &node); // Read the inode

        // Check if the inode is in use
        if (node.used == 1) {
            printf("File: %s, Size: %d blocks\n", node.name, node.size);
        }
    }

    return 0; 
}


int fs_read(char name[MAX_FILENAME], int blockNum, char buf[BLOCK_SIZE]) {
    struct inode node;
    int i, found = 0;

    for (i = 0; i < MAX_FILES; i++) {
        read_inode(i, &node);
        if (node.used == 1 && strncmp(node.name, name, MAX_FILENAME) == 0) {
            found = 1;
            break; // Found the inode
        }
    }
    if (!found) return -1; // File not found

    if (blockNum < 0 || blockNum >= node.size) return -1; // Invalid block number

    int blockAddr = node.blockPointers[blockNum];
    if (lseek(disk_fd, blockAddr * BLOCK_SIZE, SEEK_SET) < 0) return -1; // Seek error

    if (read(disk_fd, buf, BLOCK_SIZE) != BLOCK_SIZE) return -1; // Read error

    return 0; 
}


int fs_write(char name[MAX_FILENAME], int blockNum, char buf[BLOCK_SIZE]) {
    struct inode node;
    int i, found = 0;

    for (i = 0; i < MAX_FILES; i++) {
        read_inode(i, &node);
        if (node.used == 1 && strncmp(node.name, name, MAX_FILENAME) == 0) {
            found = 1;
            break; // Found the inode
        }
    }
    if (!found) return -1; // File not found

    if (blockNum < 0 || blockNum >= node.size) return -1; // Invalid block number

    int blockAddr = node.blockPointers[blockNum];
    if (lseek(disk_fd, blockAddr * BLOCK_SIZE, SEEK_SET) < 0) return -1; // Seek error

    if (write(disk_fd, buf, BLOCK_SIZE) != BLOCK_SIZE) return -1; // Write error

    return 0; 
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <disk file> <input file>\n", argv[0]);
        return 1;
    }

    // Open the disk file for both reading and writing
    disk_fd = open(argv[1], O_RDWR);
    if (disk_fd < 0) {
        perror("Error opening disk file");
        return 1;
    }

    // Open the input file for reading commands
    FILE *inputFile = fopen(argv[2], "r");
    if (inputFile == NULL) {
        perror("Error opening input file");
        close(disk_fd);
        return 1;
    }


    char diskFileName[256];
    if (fgets(diskFileName, sizeof(diskFileName), inputFile) == NULL) {
        perror("Error reading disk file name from input file");
        fclose(inputFile);
        return 1;
    }

    diskFileName[strcspn(diskFileName, "\n")] = 0;

    // Print the disk name
    printf("Disk Name: %s\n", diskFileName);

    // Process file system commands
    char command;
    char fileName[MAX_FILENAME];
    int size, blockNum;
    char buf[BLOCK_SIZE]; // Ensure that buf is defined

    while (fscanf(inputFile, " %c", &command) != EOF) {
        switch (command) {
            case 'C':
                if (fscanf(inputFile, "%s %d", fileName, &size) != 2) break;
                printf("C %s %d ", fileName, size);
                if (create(fileName, size) == 0) {
                    printf("Created file %s of this size %d blocks\n", fileName, size);
                } else {
                    printf("Error creating file %s\n", fileName);
                }
                break;
            case 'D':
                if (fscanf(inputFile, "%s", fileName) != 1) break;
                printf("D %s ", fileName);
                if (delete(fileName) == 0) {
                    printf("Deleted this file %s\n", fileName);
                } else {
                    printf("Error deleting file %s\n", fileName);
                }
                break;
            case 'L':
                printf("L list of all files on disk and their sizes\n");
                ls();
                break;
            case 'R':
                if (fscanf(inputFile, "%s %d", fileName, &blockNum) != 2) break;
                printf("R %s %d ", fileName, blockNum);
                memset(buf, 0, BLOCK_SIZE);
                if (fs_read(fileName, blockNum, buf) == 0) {
                    printf("Read this block %d from this file %s\n", blockNum, fileName);
                } else {
                    printf("Error reading from file %s\n", fileName);
                }
                break;
            case 'W':
                if (fscanf(inputFile, "%s %d", fileName, &blockNum) != 2) break;
                printf("W %s %d ", fileName, blockNum);
                memset(buf, 0, BLOCK_SIZE); // Fill buf with data to write
                if (fs_write(fileName, blockNum, buf) == 0) {
                    printf("Write to this block %d in this file %s\n", blockNum, fileName);
                } else {
                    printf("Error writing to file %s\n", fileName);
                }
                break;
            default:
                printf("Unknown command: %c\n", command);
        }
    }

    fclose(inputFile);
    close(disk_fd);
    return 0;
}
