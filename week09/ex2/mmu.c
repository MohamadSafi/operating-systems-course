#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>

#define MAX_PAGES 100  
#define MAX_FRAMES 50  
#define PAGETABLE "/tmp/ex2/pagetable"
#define PAGE_SIZE 8


struct PTE {
    bool valid;
    int frame;
    bool dirty;
    int referenced;
};




struct PTE *initialize_page_table(int fd, int num_pages) {
    struct PTE *page_table = mmap(NULL, num_pages * sizeof(struct PTE), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (page_table == MAP_FAILED) {
        perror("Error mapping page table");
        close(fd);
        exit(EXIT_FAILURE);
    }
    return page_table;
}

void sigcont(int signum) {
    // Intentionally empty handler for SIGCONT
    }


void set_handler() {
    struct sigaction sa = {
        .sa_handler = sigcont,
        .sa_flags = 0
    };
    
    sigfillset(&sa.sa_mask);

    if (sigaction(SIGCONT, &sa, NULL) == -1) {
        perror("Error setting up signal handler");
        exit(EXIT_FAILURE);
    }
}


void print_page_table(const struct PTE *page_table, int num_pages) {
    printf("\n\n####### MMU #######\n\nInitialized page table\n");
    for (int i = 0; i < num_pages; i++) {
        printf("Page %d ---> valid:%d, frame=%d, dirty=%d, referenced=%d\n",
               i, page_table[i].valid, page_table[i].frame, page_table[i].dirty, page_table[i].referenced);
    }
    printf("------------------------------\n");
}

void process_requests(struct PTE *page_table, int num_pages, pid_t pager_pid, char **argv, int argc) {
    for (int i = 2; i < argc - 1; i++) {
        char mode = argv[i][0];
        int page = argv[i][1] - '0';
        printf("\n\n####### MMU #######\n\n%s Request for page %d\n",
               mode == 'W' ? "Write" : "Read", page);

        if (page_table[page].valid) {
            if (mode == 'W') {
                page_table[page].dirty = true;
                printf("Set the dirty flag due to write request.\n");
            }
        } else {
            printf("Page fault --> Requesting page from disk.\n");
            page_table[page].referenced = getpid();
            kill(pager_pid, SIGUSR1);
            pause();
            printf("MMU resumed by pager.\n");
        }

        print_page_table(page_table, num_pages);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <num_pages> <reference_string> <pager_pid>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    int num_pages = atoi(argv[1]);
    pid_t pager_pid = (pid_t)atoi(argv[argc - 1]);

    set_handler();

    int fd = open(PAGETABLE, O_RDWR, 0666);
    if (fd == -1) {
        perror("Error opening page table file");
        exit(EXIT_FAILURE);
    }

    struct PTE *page_table = initialize_page_table(fd, num_pages);

    print_page_table(page_table, num_pages);

    process_requests(page_table, num_pages, pager_pid, argv, argc);

    kill(pager_pid, SIGUSR1);
    munmap(page_table, num_pages * sizeof(struct PTE));
    close(fd);

    printf("\nDone all requests. MMU sends SIGUSR1 to the pager and terminates.\n");

    return EXIT_SUCCESS;
}