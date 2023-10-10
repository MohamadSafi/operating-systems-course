#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_MESSAGE_SIZE 256

struct Thread {
    pthread_t id;             
    int i;                    
    char message[MAX_MESSAGE_SIZE];  
};

void *thread_function(void *arg) {
    struct Thread *thread_data = (struct Thread *)arg;
    printf("Thread %d prints message: %s\n", thread_data->i, thread_data->message);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <number_of_threads>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);

    if (n <= 0) {
        printf("Please provide a positive integer as the number of threads.\n");
        return 1;
    }

    struct Thread threads[n];

    for (int i = 0; i < n; i++) {
        threads[i].i = i + 1;
        snprintf(threads[i].message, MAX_MESSAGE_SIZE, "Hello from thread %d", threads[i].i);
        printf("Thread %d is created\n", threads[i].i);

        if (pthread_create(&threads[i].id, NULL, thread_function, &threads[i]) != 0) {
            perror("pthread_create");
            return 1;
        }

        pthread_join(threads[i].id, NULL);
    }

    return 0;
}
