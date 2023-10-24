#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>

int main() {
    int randomFd, txtFd;
    char c;
    ssize_t bytesRead;
    off_t offset = 0;
    size_t writtenChars = 0, totalSize = 500 * 1024 * 1024;
    long pageSize, chunkSize;

    // Open /dev/random for reading
    randomFd = open("/dev/random", O_RDONLY);
    if (randomFd == -1) {
        perror("Error opening /dev/random");
        return 1;
    }

    // Create text.txt for writing
    txtFd = open("text.txt", O_RDWR | O_CREAT, 0644);
    if (txtFd == -1) {
        perror("Error creating text.txt");
        close(randomFd);
        return 1;
    }

    // Extend the file to the desired size
    ftruncate(txtFd, totalSize);

    // Generate content for text.txt
    while (offset < totalSize) {
        bytesRead = read(randomFd, &c, 1);
        if (bytesRead < 1) {
            perror("Error reading from /dev/random");
            close(txtFd);
            close(randomFd);
            return 1;
        }
        if (isprint(c)) {
            lseek(txtFd, offset, SEEK_SET);
            write(txtFd, &c, 1);
            offset++;
            writtenChars++;
            if (writtenChars == 1024) {
                c = '\n';
                lseek(txtFd, offset, SEEK_SET);
                write(txtFd, &c, 1);
                offset++;
                writtenChars = 0;
            }
        }
    }

    close(randomFd);

    // Get the page size
    pageSize = sysconf(_SC_PAGESIZE);
    chunkSize = 1024 * pageSize;

    size_t totalCapitals = 0;

    while (offset > 0) {
        size_t currentChunkSize = (offset < chunkSize) ? offset : chunkSize;
        char *mappedData = mmap(NULL, currentChunkSize, PROT_READ | PROT_WRITE, MAP_SHARED, txtFd, offset - currentChunkSize);
        if (mappedData == MAP_FAILED) {
            perror("Error mmapping the file");
            close(txtFd);
            return 1;
        }
        for (size_t i = 0; i < currentChunkSize; i++) {
            if (isupper(mappedData[i])) {
                totalCapitals++;
                mappedData[i] = tolower(mappedData[i]);
            }
        }
        munmap(mappedData, currentChunkSize);
        offset -= currentChunkSize;
    }

    printf("Total capital letters: %lu\n", totalCapitals);

    close(txtFd);
    return 0;
}
