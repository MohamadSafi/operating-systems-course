#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <stdlib.h>


int main() {
    pid_t pid1, pid2;
    clock_t start_time, end_time;
    int status;

    start_time = clock();

    pid1 = fork();

    if (pid1 == 0) {
        printf("Child 1: Process ID = %d, Parent ID = %d\n", getpid(), getppid());
        end_time = clock();
        printf("Child 1 Execution Time: %f milliseconds\n", (double)abs((end_time - start_time)) / CLOCKS_PER_SEC * 1000);
    } else if (pid1 > 0) {
        pid2 = fork();

        if (pid2 == 0) {
            printf("Child 2: Process ID = %d, Parent ID = %d\n", getpid(), getppid());
            end_time = clock();
            printf("Child 2 Execution Time: %f milliseconds\n", (double)abs((end_time - start_time)) / CLOCKS_PER_SEC * 1000);
        } else if (pid2 > 0) {
            printf("Main Process: Process ID = %d\n", getpid());
            end_time = clock();
            printf("Main Process Execution Time: %f milliseconds\n", (double)(end_time - start_time) / CLOCKS_PER_SEC * 1000);
            
            waitpid(pid1, &status, 0);
            waitpid(pid2, &status, 0);
        } else {
            perror("Failed to fork second child");
            return 1;
        }
    } else {
        perror("Failed to fork first child");
        return 1;
    }

    return 0;
}
