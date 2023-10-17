#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int shouldRead = 0; 

void handle_SIGUSR1(int signum) {
    shouldRead = 1; 
}

void handle_SIGUSR2(int signum) {
    printf("Process terminating...\n");
    exit(0);
}

int main() {
    FILE *pid_file = fopen("/var/run/agent.pid", "w");
    if (pid_file == NULL) {
        perror("Error writing PID file");
        exit(1);
    }
    fprintf(pid_file, "%d", getpid());
    fclose(pid_file);

    signal(SIGUSR1, handle_SIGUSR1);
    signal(SIGUSR2, handle_SIGUSR2);

    while (1) {
        if (shouldRead) {
            char buffer[100];
            FILE *file = fopen("text.txt", "r");
            if (file != NULL) {
                while (fgets(buffer, sizeof(buffer), file)) {
                    printf("%s", buffer);
                }
                fclose(file);
            }
            shouldRead = 0; 
        }

        sleep(1);
    }

    return 0;
}
