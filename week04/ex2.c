#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

#define VECTOR_SIZE 120

int randInRange() {
    return rand() % 100;
}

int main() {
    srand(time(NULL));
    int n;
    printf("Enter the number of processes (n): ");
    scanf("%d", &n);

    if (n < 1 || n > 32 || (n & (n - 1)) != 0) {
        printf("n must be a power of 2 between 1 and 32.\n");
        return 1;
    }

    int u[VECTOR_SIZE], v[VECTOR_SIZE];
    for (int i = 0; i < VECTOR_SIZE; i++) {
        u[i] = randInRange();
        v[i] = randInRange();
    }

    FILE *file = fopen("temp.txt", "w+");
    if (file == NULL) {
        perror("Error opening temp.txt");
        return 1;
    }

    int chunk_size = VECTOR_SIZE / n;

    for (int i = 0; i < n; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            int start = i * chunk_size;
            int end = (i == n - 1) ? VECTOR_SIZE : (i + 1) * chunk_size;
            int partial_sum = 0;

            for (int j = start; j < end; j++) {
                partial_sum += u[j] * v[j];
            }

            fprintf(file, "%d\n", partial_sum);
            fclose(file);
            exit(0);
        } else if (pid < 0) {
            perror("Fork error");
            return 1;
        }
    }

    for (int i = 0; i < n; i++) {
        wait(NULL);
    }

    fseek(file, 0, SEEK_SET);

    int final_result = 0;
    int temp;
    while (fscanf(file, "%d", &temp) != EOF) {
        final_result += temp;
    }

    printf("The dot product of u and v is: %d\n", final_result);

    fclose(file);

    return 0;
}
