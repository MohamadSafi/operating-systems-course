#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>

int main() {
    FILE *fp = fopen("/tmp/ex1.pid", "w");
    if (fp == NULL) {
        perror("Error opening /tmp/ex1.pid");
        exit(EXIT_FAILURE);
    }
    fprintf(fp, "%d", getpid());
    fclose(fp);

    char password[13] = "pass:"; 
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd == -1) {
        perror("Error opening /dev/urandom");
        exit(EXIT_FAILURE);
    }

    for (int i = 5; i < 12; i++) {
        char random_char;
        while (1) {
            read(fd, &random_char, 1);
            if (random_char >= 32 && random_char <= 126) { 
                password[i] = random_char;
                break;
            }
        }
    }
    password[12] = '\0';
    close(fd);

    char *addr = mmap(NULL, 13, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (addr == MAP_FAILED) {
        perror("Error mmapping");
        exit(EXIT_FAILURE);
    }
    strncpy(addr, password, 13);

    printf("Generated Password: %s\n", password);

    while (1) {
        sleep(1);  
    }
    return 0;
}
