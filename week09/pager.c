#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <fcntl.h>
#include <limits.h>

#define PAGE_SIZE 8
#define MAX_PAGES 100  
#define MAX_FRAMES 50  
#define PAGETABLE "/tmp/ex2/pagetable"


struct PTE {
    bool valid;
    int frame;
    bool dirty;
    int referenced;
};

char **RAM;
char **disk;
int *free_frame_list;
int num_pages;
int num_frames;
struct PTE *page_table;
int disk_access_count = 0;
unsigned char *age_vector; // Global declaration of the age vector


int (*page_replacement_algorithm)(struct PTE*);

void handle_sigterm(int signum) {
    printf("%d disk accesses in total\n", disk_access_count);
    printf("Pager is terminating\n");
    exit(0);
}

void initialize_disk(int page_count) {
    int urandom_fd = open("/dev/urandom", O_RDONLY);
    if (urandom_fd < 0) {
        perror("Failed to open /dev/urandom");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < page_count; i++) {
        for (int j = 0; j < PAGE_SIZE; j++) {
            unsigned char random_byte;
            ssize_t result = read(urandom_fd, &random_byte, sizeof(random_byte));
            if (result < 0) {
                perror("Failed to read from /dev/urandom");
                close(urandom_fd);
                exit(EXIT_FAILURE);
            }
            disk[i][j] = ' ' + (random_byte % 95);
        }
    }

    close(urandom_fd);
}


int find_requested_page() {
    for (int i = 0; i < num_pages; i++) {
        if (page_table[i].referenced) {
            return i;
        }
    }
    return -1;
}

int find_available_frame() {
    for (int i = 0; i < num_frames; i++) {
        if (free_frame_list[i]) {
            return i;
        }
    }
    return -1;
}

int select_victim_page() {
    printf("No free frames in RAM, executing page replacement algorithm\n");
    int victim_page = page_replacement_algorithm(page_table);

    printf("Selected victim page %d\n", victim_page);
    return page_table[victim_page].frame;
}

void replace_page_in_frame(int requested_page, int available_frame) {
    printf("Replace with requested page %d in frame %d\n", requested_page, available_frame);
}

void copy_page_to_ram(int requested_page, int available_frame) {
    printf("Copying from disk page %d to RAM frame %d\n", requested_page, available_frame);
    memcpy(RAM[available_frame], disk[requested_page], PAGE_SIZE);
    disk_access_count++;
}

void update_page_table(int requested_page, int available_frame) {
    page_table[requested_page] = (struct PTE){.valid = true, .frame = available_frame};
    free_frame_list[available_frame] = 0;
}

void print_ram_state() {
    printf("RAM State:\n");
    for (int i = 0; i < num_frames; i++) {
        printf("Frame %d --> %s\n", i, RAM[i]);
    }
}

void resume_mmu_process(int requesting_pid) {
    kill(requesting_pid, SIGCONT);
}

void handle_sigusr1(int signum) {
    printf("\n\n####### Pager #######\n\n");

    int requested_page = find_requested_page();

    if (requested_page == -1) {
        printf("%d disk accesses in total\n", disk_access_count);
        printf("Pager is terminated");
        return;
    }

    int requesting_pid = page_table[requested_page].referenced;
    printf("A disk access request from MMU Process (pid=%d)\n", requesting_pid);
    printf("Page %d is referenced\n", requested_page);

    int available_frame = find_available_frame();

    if (available_frame == -1) {
        available_frame = select_victim_page();
    }

    replace_page_in_frame(requested_page, available_frame);
    copy_page_to_ram(requested_page, available_frame);
    update_page_table(requested_page, available_frame);

    print_ram_state();

    printf("Total disk accesses: %d\n", disk_access_count);
    printf("Resuming MMU process\n");
    printf("------------------------------\n\n\n");

    resume_mmu_process(requesting_pid);
}


void configure_signal_handler(int signal, void (*handler)(int)) {
    struct sigaction sa;
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(signal, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
}

void setup_signals() {
    configure_signal_handler(SIGUSR1, handle_sigusr1);
}

void initialize_memory_structures() {
    RAM = malloc(num_frames * sizeof(char *));
    for (int i = 0; i < num_frames; i++) {
        RAM[i] = malloc(PAGE_SIZE);
    }

    disk = malloc(num_pages * sizeof(char *));
    for (int i = 0; i < num_pages; i++) {
        disk[i] = malloc(PAGE_SIZE);
    }

    free_frame_list = malloc(num_frames * sizeof(int));
    for (int i = 0; i < num_frames; i++) {
        free_frame_list[i] = 1; // All frames are initially free
    }


    age_vector = malloc(num_pages * sizeof(unsigned char));
    if (age_vector == NULL) {
        perror("Failed to allocate age vector");
        exit(EXIT_FAILURE);
    }
    memset(age_vector, 0, num_pages * sizeof(unsigned char)); // Initialize with zeros
}


void cleanup_memory_structures() {
    for (int i = 0; i < num_frames; i++) {
        free(RAM[i]);
    }
    free(RAM);

    for (int i = 0; i < num_pages; i++) {
        free(disk[i]);
    }
    free(disk);

    free(free_frame_list);

        free(age_vector);
}

void print_initial_state() {
    printf("Initialized page table\n");
    for (int i = 0; i < num_pages; i++) {
        printf("Page %d ---> valid:%d, frame=%d, dirty=%d, referenced=%d\n", i, page_table[i].valid, page_table[i].frame, page_table[i].dirty, page_table[i].referenced);
    }
    printf("------------------------------\n");

    printf("Initialized RAM\nRAM State:\n");
    for (int i = 0; i < num_frames; i++) {
        printf("Frame %d --> %s\n", i, RAM[i]);
    }
    printf("------------------------------\n");

    printf("Initialized disk\nDisk State:\n");
    for (int i = 0; i < num_pages; i++) {
        printf("Page %d --> %s\n", i, disk[i]);
    }
    printf("------------------------------\n\n\n");
}

int random(struct PTE* page_table) {
    srand(time(NULL));
    int victim_page;
    do {
        victim_page = rand() % num_pages; // Get a random page number
    } while (!page_table[victim_page].valid); // Ensure the page is in memory

    return victim_page;
}

int nfu(struct PTE* page_table) {
    int victim_page = -1;
    int min_referenced = INT_MAX; // Use limits.h for INT_MAX value

    for (int i = 0; i < num_pages; i++) {
        if (page_table[i].valid && page_table[i].referenced < min_referenced) {
            min_referenced = page_table[i].referenced;
            victim_page = i;
        }
    }

    return victim_page;
}

int aging(struct PTE* page_table) {
    int victim_page = -1;
    unsigned char min_age = UCHAR_MAX;
    for (int i = 0; i < num_pages; i++) {
        if (page_table[i].valid) {
            age_vector[i] >>= 1;
            if (page_table[i].referenced) {
                age_vector[i] |= 0x80;
                page_table[i].referenced = 0;
            }
            if (age_vector[i] < min_age) {
                min_age = age_vector[i];
                victim_page = i;
            }
        }
    }
    return victim_page;
}



int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s <num_pages> <num_frames> <algorithm>\n", argv[0]);
        exit(1);
    }

    num_pages = atoi(argv[1]);
    num_frames = atoi(argv[2]);
        const char* algorithm = argv[3];

            // Select the page replacement algorithm based on user input
    if (strcmp(algorithm, "random") == 0) {
        page_replacement_algorithm = random;
        printf("Selected Random page replacement algorithm.\n");
    } else if (strcmp(algorithm, "nfu") == 0) {
        page_replacement_algorithm = nfu;
        printf("Selected NFU page replacement algorithm.\n");
    } else if (strcmp(algorithm, "aging") == 0) {
        page_replacement_algorithm = aging;
        printf("Selected Aging page replacement algorithm.\n");
    } else {
        fprintf(stderr, "Invalid page replacement algorithm: %s\n", algorithm);
        exit(EXIT_FAILURE);
    }

    setup_signals();

    printf("\n\n####### Pager #######\n\n");

    initialize_memory_structures();

    int fd = open(PAGETABLE, O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
        perror("Error opening page table file");
        exit(1);
    }
    ftruncate(fd, num_pages * sizeof(struct PTE));
    page_table = mmap(NULL, num_pages * sizeof(struct PTE), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (page_table == MAP_FAILED) {
        perror("Error mapping page table");
        close(fd);
        exit(1);
    }

    for (int i = 0; i < num_pages; i++) {
        page_table[i] = (struct PTE){0};
    }

    print_initial_state();
    initialize_disk(num_pages);

    while (1) {
        pause();
    }

    cleanup_memory_structures();
    munmap(page_table, num_pages * sizeof(struct PTE));
    close(fd);

    return 0;
}
