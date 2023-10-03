#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <n>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);

    for (int i = 0; i < n; i++) {
        pid_t child_pid = fork();

        if (child_pid == -1) {
            perror("fork");
            return 1;
        }

        if (child_pid == 0) {
            printf("Child process (PID: %d)\n", getpid());
            sleep(5);
            exit(0);
        } else {            printf("Parent process (PID: %d) created child process (PID: %d)\n", getpid(), child_pid);
        }
    }

    return 0;
}
