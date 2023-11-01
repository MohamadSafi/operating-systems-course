#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>

int main() {
    struct rusage r_usage;

    for(int i = 0; i < 10; i++) {
        char *memory = malloc(10 * 1024 * 1024);
        if(!memory) {
            printf("Memory allocation failed!\n");
            return 1;
        }

        memset(memory, 0, 10 * 1024 * 1024);

        getrusage(RUSAGE_SELF, &r_usage);
        printf("Memory usage: %ld KB\n", r_usage.ru_maxrss);

        sleep(1);
    }

    return 0;
}
