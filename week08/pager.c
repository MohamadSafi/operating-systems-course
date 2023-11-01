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
char RAM[MAX_FRAMES][PAGE_SIZE];
char Disk[MAX_PAGES][PAGE_SIZE];
pid_t mmu_pid;
int num_pages;
int num_frames;
static int next_frame = 0;
int disk_accesses = 0;



void print_page_table() {
    printf("-------------------------\n");
    printf("Initialized page table\n");
    for (int i = 0; i < num_pages; i++) {
        printf("Page %d ---> valid=%d, frame=%d, dirty=%d, referenced=%d\n",
               i, page_table[i].valid, page_table[i].frame,
               page_table[i].dirty, page_table[i].referenced);
    }
    printf("-------------------------\n");
}

void print_RAM() {
    printf("Initialized RAM\n");
    printf("RAM array\n");
    for (int i = 0; i < num_frames; i++) {
        printf("Frame %d ---> %s\n", i, RAM[i]);
    }
    printf("-------------------------\n");
}

void print_disk() {
    printf("Initialized disk\n");
    printf("Disk array\n");
    for (int i = 0; i < num_pages; i++) {
        printf("Page %d ----> %s\n", i, Disk[i]);
    }
    printf("-------------------------\n");
}




void signal_handler(int signum) {
    int page_to_load = -1;

    if (signum == SIG_PAGE_FAULT) {
        printf("Pager received page fault signal.\n");

        for (int i = 0; i < num_pages; i++) {
            if (page_table[i].referenced && !page_table[i].valid) {
                page_to_load = i;
                page_table[i].referenced = false;
                break;
            }
        }
    } else if (signum == SIG_TERMINATE) {
        printf("%d disk accesses in total\n", disk_accesses);
        printf("Pager is terminated\n");
        exit(0);
    }

    if (page_to_load != -1) {
        printf("A disk access request from MMU Process (pid=%d)\n", mmu_pid);
        printf("Page %d is referenced\n", page_to_load);
        disk_accesses++;

        if (next_frame < num_frames) {
            printf("We can allocate it to free frame %d\n", next_frame);
        } else {
            printf("We do not have free frames in RAM\n");
            int victim_page = rand() % num_pages; 
            printf("Chose a random victim page %d\n", victim_page);
            if (page_table[victim_page].dirty) {
                printf("Evicting dirty page %d from frame %d back to disk\n", victim_page, page_table[victim_page].frame);
                strcpy(Disk[victim_page], RAM[page_table[victim_page].frame]);
                disk_accesses++;
            }
            printf("Replace/Evict it with page %d to be allocated to frame %d\n", page_to_load, page_table[victim_page].frame);
            next_frame = page_table[victim_page].frame;
            page_table[victim_page].valid = false;
            page_table[victim_page].frame = -1;
        }
        
        printf("Copy data from the disk (page=%d) to RAM (frame=%d)\n", page_to_load, next_frame);
        strcpy(RAM[next_frame], Disk[page_to_load]);
        page_table[page_to_load].valid = true;
        page_table[page_to_load].frame = next_frame;
        printf("RAM array\n");
        for (int i = 0; i < num_frames; i++) {
            printf("Frame %d ---> %s\n", i, RAM[i]);
        }
        printf("disk accesses is %d so far\n", disk_accesses);
        printf("-------------------------\n");
        next_frame = (next_frame + 1) % num_frames;
        kill(mmu_pid, SIG_PAGE_LOADED);
    }
}


int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: ./pager <num_pages> <num_frames>\n");
        exit(1);
    }

    num_pages = atoi(argv[1]);
    num_frames = atoi(argv[2]);

    if (num_pages > MAX_PAGES || num_frames > MAX_FRAMES || num_frames > num_pages) {
        fprintf(stderr, "Invalid number of pages or frames\n");
        exit(1);
    }

    int fd = open("page_table.mmap", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    ftruncate(fd, sizeof(struct PTE) * num_pages);
    page_table = (struct PTE *)mmap(NULL, sizeof(struct PTE) * num_pages, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);

    for (int i = 0; i < num_pages; i++) {
        page_table[i].valid = false;
        page_table[i].frame = -1;  
        page_table[i].dirty = false;
        page_table[i].referenced = false;
    }

const char* diskContents[] = {"gEefwaq", "kjQ2eeq", "43R2e2e", "jji2u32"};
for (int i = 0; i < num_pages; i++) {
    snprintf(Disk[i], PAGE_SIZE - 1, "%s", diskContents[i]);
}


    signal(SIG_PAGE_FAULT, signal_handler);
    signal(SIG_TERMINATE, signal_handler);

    printf("Pager initialized. Waiting for MMU signals...\n");

    print_page_table();
    print_RAM();
    print_disk();

    while (1) {
        pause();
    }

    return 0;
}
