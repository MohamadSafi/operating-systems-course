#include <stdio.h>
#include <stdlib.h>

int num_processes = 5;  
int num_resources = 3;  
void read_data(const char* filename, int E[], int A[], int C[][num_resources], int R[][num_resources]) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Unable to open file");
        exit(1);
    }

    for (int i = 0; i < num_resources; i++)
        fscanf(file, "%d", &E[i]);
    for (int i = 0; i < num_resources; i++)
        fscanf(file, "%d", &A[i]);

    for (int i = 0; i < num_processes; i++)
        for (int j = 0; j < num_resources; j++)
            fscanf(file, "%d", &C[i][j]);

    for (int i = 0; i < num_processes; i++)
        for (int j = 0; j < num_resources; j++)
            fscanf(file, "%d", &R[i][j]);

    fclose(file);
}

void print_deadlocked_processes(int finish[]) {
    printf("Deadlocked processes: ");
    for (int i = 0; i < num_processes; i++) {
        if (!finish[i]) {
            printf("P%d ", i);
        }
    }
    printf("\n");
}

int bankers_algorithm(int E[], int A[], int C[][num_resources], int R[][num_resources]) {
    int finish[num_processes];
    for (int i = 0; i < num_processes; i++)
        finish[i] = 0;

    int work[num_resources];
    for (int i = 0; i < num_resources; i++)
        work[i] = A[i];

    int count = 0;
    while (count < num_processes) {
        int found = 0;
        for (int p = 0; p < num_processes; p++) {
            if (finish[p] == 0) {
                int j;
                for (j = 0; j < num_resources; j++)
                    if (R[p][j] > work[j])
                        break;

                if (j == num_resources) {
                    for (int k = 0; k < num_resources; k++)
                        work[k] += C[p][k];
                    
                    finish[p] = 1;
                    found = 1;
                    count++;
                }
            }
        }

        if (found == 0) {
            print_deadlocked_processes(finish);
            return 0;
        }
    }

    return 1;
}

int main() {
    int E[num_resources], A[num_resources];
    int C[num_processes][num_resources], R[num_processes][num_resources];

    read_data("input.txt", E, A, C, R);

    if (bankers_algorithm(E, A, C, R)) {
        printf("No deadlock detected.\n");
    } else {
        printf("Deadlock detected.\n");
    }

    return 0;
}
