#include <stdio.h>
#include <stdlib.h>

const int tri(int *p, int n) {
    int temp = 0;
    if (n == 0) {
        return p[0];
    } else if (n == 1) {
        return p[1];
    } else if (n == 2) {
        return p[2];
    } else {
        for (int i = 3; i <= n; i++) {
            temp = p[0] + p[1] + p[2];
            p[0] = p[1];
            p[1] = p[2];
            p[2] = temp;
        }
        return p[2];
    }
}

int main() {
    const int x = 1;
    const int *q = &x; 

    int *p = (int *)malloc(3 * sizeof(int));
    if (p == NULL) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    p[0] = x;
    p[1] = x;
    p[2] = 2 * x;

    printf("Memory addresses:\n");
    for (int i = 0; i < 3; i++) {
        printf("p[%d] address: %p\n", i, (void *)&p[i]);
    }

    if (&p[0] + 1 == &p[1] && &p[1] + 1 == &p[2]) {
        printf("The cells are contiguous.\n");
    } else {
        printf("The cells are not contiguous.\n");
    }

    int n = 5; 
    int result = tri(p, n);
    printf("Tribonacci number for n=%d: %d\n", n, result);

    free(p);

    return 0;
}
