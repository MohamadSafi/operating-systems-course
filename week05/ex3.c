#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

bool is_prime(int n) {
    if (n <= 1) return false;
    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0)
            return false;
    }
    return true;
}

typedef struct prime_request {
    int a, b;
} prime_request;

void *prime_counter(void *arg) {
    prime_request *req = (prime_request *)arg;
    int count = 0;
    for (int i = req->a; i < req->b; i++) {
        if (is_prime(i)) {
            count++;
        }
    }
    int *result = malloc(sizeof(int));
    *result = count;
    return (void *)result;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <n> <m>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    int m = atoi(argv[2]);

    pthread_t threads[m];
    prime_request requests[m];
    int results[m];
    int total_primes = 0;

    int sub_interval_size = n / m;

    for (int i = 0; i < m; i++) {
        requests[i].a = i * sub_interval_size;
        requests[i].b = (i == m - 1) ? n : (i + 1) * sub_interval_size;

        if (pthread_create(&threads[i], NULL, prime_counter, &requests[i]) != 0) {
            perror("pthread_create");
            return 1;
        }
    }

    for (int i = 0; i < m; i++) {
        int *result;
        if (pthread_join(threads[i], (void **)&result) != 0) {
            perror("pthread_join");
            return 1;
        }
        results[i] = *result;
        free(result);
    }

    for (int i = 0; i < m; i++) {
        total_primes += results[i];
    }

    printf("Number of primes in the range [0, %d) is %d\n", n, total_primes);

    return 0;
}
