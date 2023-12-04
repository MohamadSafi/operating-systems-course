#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

pthread_mutex_t *mutexes;
bool *is_thread_active;
pthread_mutex_t print_mutex; 

int m, n;  

 
typedef struct {
    int id;
    bool *resources; 
} ThreadInfo;

void *thread_routine(void *arg) {
    ThreadInfo *info = (ThreadInfo *)arg;

    pthread_mutex_lock(&print_mutex);
    printf("Thread %d is created\n", info->id);
    pthread_mutex_unlock(&print_mutex);

    for(int i = 0; i < m; i++) {
        if (info->resources[i]) {
            pthread_mutex_lock(&print_mutex);
            printf("Thread %d tries to lock mutex %d\n", info->id, i);
            pthread_mutex_unlock(&print_mutex);

            pthread_mutex_lock(&mutexes[i]);
            
            pthread_mutex_lock(&print_mutex);
            printf("Thread %d locked mutex %d\n", info->id, i);
            pthread_mutex_unlock(&print_mutex);

            sleep(rand() % 5); 

            pthread_mutex_unlock(&mutexes[i]);

            pthread_mutex_lock(&print_mutex);
            printf("Thread %d unlocked mutex %d\n", info->id, i);
            pthread_mutex_unlock(&print_mutex);
        }
    }

    pthread_mutex_lock(&print_mutex);
    printf("Thread %d has finished\n", info->id);
    pthread_mutex_unlock(&print_mutex);

    is_thread_active[info->id] = false;

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <num_resources> <num_threads>\n", argv[0]);
        return 1;
    }

    m = atoi(argv[1]);
    n = atoi(argv[2]);

    mutexes = malloc(m * sizeof(pthread_mutex_t));
    is_thread_active = calloc(n, sizeof(bool));
    pthread_mutex_init(&print_mutex, NULL);

    for (int i = 0; i < m; ++i) {
        pthread_mutex_init(&mutexes[i], NULL);
    }

    ThreadInfo *thread_infos[n];
    memset(thread_infos, 0, sizeof(thread_infos));

    FILE *file = fopen("input.txt", "r");
    if (file == NULL) {
        perror("Unable to open file");
        return 1;
    }

    pthread_t threads[n];
    int thread_id, resource_id;
    while (fscanf(file, "%d %d", &thread_id, &resource_id) != EOF) {
        if (thread_id >= n || resource_id >= m) {
            fprintf(stderr, "Invalid thread or resource ID\n");
            continue;
        }

        if (!is_thread_active[thread_id]) {
            thread_infos[thread_id] = malloc(sizeof(ThreadInfo));
            thread_infos[thread_id]->id = thread_id;
            thread_infos[thread_id]->resources = calloc(m, sizeof(bool));
            is_thread_active[thread_id] = true;

            pthread_create(&threads[thread_id], NULL, thread_routine, thread_infos[thread_id]);
        }

        thread_infos[thread_id]->resources[resource_id] = true;
    }

    fclose(file);

    for (int i = 0; i < n; ++i) {
        if (is_thread_active[i]) {
            pthread_join(threads[i], NULL);
        }
    }

    for (int i = 0; i < m; ++i) {
        pthread_mutex_destroy(&mutexes[i]);
    }
    for (int i = 0; i < n; ++i) {
        if (thread_infos[i] != NULL) {
            free(thread_infos[i]->resources);
            free(thread_infos[i]);
        }
    }
    pthread_mutex_destroy(&print_mutex);
    free(mutexes);
    free(is_thread_active);

    return 0;
}
