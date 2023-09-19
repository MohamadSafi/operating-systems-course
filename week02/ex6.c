#include <stdio.h>

int main() {
    int rows = 7; 

    printf("Shape 1:\n");
    for (int i = 1; i <= rows; i++) {
        for (int j = 1; j <= i; j++) {
            printf("*");
        }
        printf("\n");
    }

    printf("\nShape 2:\n");
    for (int i = 1; i <= 4; i++) {
        for (int j = 1; j <= i; j++) {
            printf("*");
        }
        printf("\n");
    }


    for (int i = 3; i >= 1; i--) {
        for (int j = 1; j <= i; j++) {
            printf("*");
        }
        printf("\n");
    }

    printf("\nShape 3:\n");
    for (int i = 1; i <= rows; i++) {
        for (int j = 1; j <= rows; j++) {
            printf("*");
        }
        printf("\n");
    }

    return 0;
}
