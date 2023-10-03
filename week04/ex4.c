#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_INPUT_LENGTH 100
#define MAX_ARGS 10

void parse_input(char *input, char *command, char **args) {
    int i = 0;
    char *token = strtok(input, " \t\n");
    while (token != NULL && i < MAX_ARGS) {
        args[i] = token;
        token = strtok(NULL, " \t\n");
        i++;
    }
    args[i] = NULL;
    strcpy(command, args[0]);
}

int main() {
    char input[MAX_INPUT_LENGTH];
    char command[MAX_INPUT_LENGTH];
    char *args[MAX_ARGS];
    int background = 0;

    while (1) {
        printf("MyShell> ");
        fgets(input, MAX_INPUT_LENGTH, stdin);

        if (strlen(input) == 1) {
            continue;
        }

        if (input[strlen(input) - 2] == '&') {
            background = 1;
            input[strlen(input) - 2] = '\0'; 
        } else {
            background = 0;
            input[strlen(input) - 1] = '\0'; 
        }

        parse_input(input, command, args);

        pid_t pid = fork();

        if (pid < 0) {
            perror("Fork error");
            exit(1);
        } else if (pid == 0) {
            if (execvp(command, args) == -1) {
                perror("Execution error");
                exit(1);
            }
        } else {
            if (!background) {
                int status;
                waitpid(pid, &status, 0);
            }
        }
    }

    return 0;
}
