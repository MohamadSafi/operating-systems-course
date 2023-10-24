#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <limits.h>

#define MAX_SIZE 10000000

unsigned int memory[MAX_SIZE];

void clear_memory() {
    for (unsigned int i = 0; i < MAX_SIZE; i++) {
        memory[i] = 0;
    }
}

bool first_fit(unsigned int adrs, unsigned int size) {
    for (unsigned int i = 0; i <= MAX_SIZE - size; i++) {
        bool can_allocate = true;
        for (unsigned int j = i; j < i + size; j++) {
            if (memory[j] != 0) {
                can_allocate = false;
                break;
            }
        }
        if (can_allocate) {
            for (unsigned int j = i; j < i + size; j++) {
                memory[j] = adrs;
            }
            return true;
        }
    }
    return false;
}

bool best_fit(unsigned int adrs, unsigned int size) {
    unsigned int best_index = UINT_MAX;
    unsigned int best_size = UINT_MAX;
    unsigned int current_size = 0;
    
    for (unsigned int i = 0; i < MAX_SIZE; i++) {
        if (memory[i] == 0) {
            current_size++;
        } else {
            if (current_size >= size && current_size < best_size) {
                best_size = current_size;
                best_index = i - current_size;
            }
            current_size = 0;
        }
    }
    if (best_index != UINT_MAX) {
        for (unsigned int i = best_index; i < best_index + size; i++) {
            memory[i] = adrs;
        }
        return true;
    }
    return false;
}

bool worst_fit(unsigned int adrs, unsigned int size) {
    unsigned int worst_index = UINT_MAX;
    unsigned int worst_size = 0;
    unsigned int current_size = 0;

    for (unsigned int i = 0; i < MAX_SIZE; i++) {
        if (memory[i] == 0) {
            current_size++;
        } else {
            if (current_size > worst_size) {
                worst_size = current_size;
                worst_index = i - current_size;
            }
            current_size = 0;
        }
    }
    if (worst_index != UINT_MAX) {
        for (unsigned int i = worst_index; i < worst_index + size; i++) {
            memory[i] = adrs;
        }
        return true;
    }
    return false;
}

void clear(unsigned int adrs) {
    for (unsigned int i = 0; i < MAX_SIZE; i++) {
        if (memory[i] == adrs) {
            memory[i] = 0;
        }
    }
}

double run_queries(bool (*allocator)(unsigned int, unsigned int)) {
    FILE* fp = fopen("queries.txt", "r");
    if (!fp) {
        perror("Failed to open queries.txt");
        return -1.0;
    }

    clear_memory();
    char query[20];
    unsigned int adrs, size;
    unsigned int queries_count = 0;

    clock_t start = clock();

    while (fscanf(fp, "%s", query) != EOF && query[0] != 'e') {
        if (query[0] == 'a') {
            fscanf(fp, "%u %u", &adrs, &size);
            allocator(adrs, size);
        } else {
            fscanf(fp, "%u", &adrs);
            clear(adrs);
        }
        queries_count++;
    }

    clock_t end = clock();
    fclose(fp);
    double time_taken = ((double)end - start) / CLOCKS_PER_SEC;
    return (double)queries_count / time_taken;
}

int main() {
    double ff_throughput = run_queries(first_fit);
    double bf_throughput = run_queries(best_fit);
    double wf_throughput = run_queries(worst_fit);

    printf("First Fit Throughput: %lf\n", ff_throughput);
    printf("Best Fit Throughput: %lf\n", bf_throughput);
    printf("Worst Fit Throughput: %lf\n", wf_throughput);

    FILE* out = fopen("ex1.txt", "w");
    if (out) {
        fprintf(out, "First Fit Throughput: %lf\n", ff_throughput);
        fprintf(out, "Best Fit Throughput: %lf\n", bf_throughput);
        fprintf(out, "Worst Fit Throughput: %lf\n", wf_throughput);
        fclose(out);
    } else {
        perror("Failed to write to ex1.txt");
    }

    return 0;
}
