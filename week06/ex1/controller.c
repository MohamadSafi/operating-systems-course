#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

int agent_pid;  

void handle_SIGINT(int signum) {
    kill(agent_pid, SIGUSR2); 
    printf("Controller terminating...\n");
    exit(0); 
}

int main() {
    FILE *pid_file = fopen("/var/run/agent.pid", "r");
    if (pid_file == NULL) {
        perror("Error: No agent found.");
        exit(1);
    }

    fscanf(pid_file, "%d", &agent_pid);
    fclose(pid_file);

    printf("Agent found.\n");

    signal(SIGINT, handle_SIGINT);  

    while (1) {
        printf("Choose a command {“read”, “exit”, “stop”, “continue”} to send to the agent: ");
        char command[20];
        scanf("%s", command);

        if (strcmp(command, "exit") == 0) {
            kill(agent_pid, SIGUSR2);
            exit(0);
        } else if (strcmp(command, "stop") == 0) {
            kill(agent_pid, SIGSTOP);
        } else if (strcmp(command, "continue") == 0) {
            kill(agent_pid, SIGCONT);
        } else if (strcmp(command, "read") == 0) {
            kill(agent_pid, SIGUSR1);
        } else {
            printf("Invalid command. Choose from {“read”, “exit”, “stop”, “continue”}.\n");
        }
    }

    return 0;
}
