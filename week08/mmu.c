#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>

#define PAGE_SIZE 8
#define MAX_PAGES 100  
#define MAX_FRAMES 50  

struct PTE {
    bool valid;    
    int frame;     
    bool dirty;    
    int referenced; 
};



#define SIG_PAGE_FAULT 30
#define SIG_PAGE_LOADED 31
#define SIG_TERMINATE 32



struct PTE *page_table;
pid_t pager_pid;
int num_pages;
int disk_accesses = 0;


void signal_handler(int signum) {
    if (signum == SIG_PAGE_LOADED) {
        printf("Resume MMU process\n");
        printf("-------------------------\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc < 5) {
        fprintf(stderr, "Usage: ./mmu <num_pages> <pager_pid> [sequence of operations]\n");
        exit(1);
    }

    num_pages = atoi(argv[1]);
    pager_pid = atoi(argv[2]);

    int fd = open("page_table.mmap", O_RDWR);
    if (fd == -1) {
        perror("Failed to open page_table.mmap");
        exit(1);
    }
    page_table = (struct PTE *)mmap(NULL, sizeof(struct PTE) * num_pages, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);

    signal(SIG_PAGE_LOADED, signal_handler);


    for (int i = 3; i < argc; i++) {
        char op = argv[i][0];
        int page_num = atoi(argv[i] + 1);

        if (page_num >= num_pages) {
            fprintf(stderr, "Invalid page number: %d\n", page_num);
            continue;
        }

        if (!page_table[page_num].valid) {
            printf("MMU: Page fault for page %d.\n", page_num);
            page_table[page_num].referenced = true;
            kill(pager_pid, SIG_PAGE_FAULT);
            pause(); 
        }

        page_table[page_num].referenced = true;
        if (op == 'W' || op == 'w') {
            page_table[page_num].dirty = true;
        }
    }

    printf("%d disk accesses in total\n", disk_accesses);
    kill(pager_pid, SIG_TERMINATE);
    printf("Pager is terminated\n");
    munmap(page_table, sizeof(struct PTE) * num_pages);
    printf("MMU terminating...\n");


    return 0;
}
