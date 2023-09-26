#include <stdio.h>
#include <stdlib.h>
#include <float.h>  
#include <limits.h>

void* aggregate(void* base, size_t size, int n, void* initial_value, void* (*opr)(const void*, const void*)) {
    void* result = initial_value;

    for (int i = 0; i < n; i++) {
        result = opr(result, (char*)base + i * size);
    }

    return result;
}

void* max_op(const void* a, const void* b) {
    if (*(int*)a > *(int*)b) {
        return (void*)a;
    } else {
        return (void*)b;
    }
}

void* add_op(const void* a, const void* b) {
    void* result = malloc(sizeof(int));
    *(int*)result = *(int*)a + *(int*)b;
    return result;
}

void* mul_op(const void* a, const void* b) {
    void* result = malloc(sizeof(int));
    *(int*)result = *(int*)a * *(int*)b;
    return result;
}

int main() {
    double doubleArray[5] = {1.5, 2.5, 3.5, 4.5, 5.5};
    int intArray[5] = {1, 2, 3, 4, 5};

    printf("Double Array:\n");
    double initial_double = 0.0;
    double* doubleResult = (double*)aggregate(doubleArray, sizeof(double), 5, &initial_double, add_op);
    printf("Sum: %lf\n", *doubleResult);

    initial_double = 1.0;
    doubleResult = (double*)aggregate(doubleArray, sizeof(double), 5, &initial_double, mul_op);
    printf("Product: %lf\n", *doubleResult);

    initial_double = DBL_MIN;  // Use DBL_MIN from float.h
    doubleResult = (double*)aggregate(doubleArray, sizeof(double), 5, &initial_double, max_op);
    printf("Max: %lf\n", *doubleResult);

    printf("\nInteger Array:\n");
    int initial_int = 0;
    int* intResult = (int*)aggregate(intArray, sizeof(int), 5, &initial_int, add_op);
    printf("Sum: %d\n", *intResult);

    initial_int = 1;
    intResult = (int*)aggregate(intArray, sizeof(int), 5, &initial_int, mul_op);
    printf("Product: %d\n", *intResult);

    initial_int = INT_MIN;
    intResult = (int*)aggregate(intArray, sizeof(int), 5, &initial_int, max_op);
    printf("Max: %d\n", *intResult);

    free(doubleResult);
    free(intResult);

    return 0;
}
