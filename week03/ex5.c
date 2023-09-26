#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <limits.h>
#include <float.h>

// Define the aggregation operations
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

void* vaggregate(void* (*opr)(const void*, const void*), int n, ...) {
    va_list args;
    va_start(args, n);
    
    void* initial_value = va_arg(args, void*);
    
    void* result = initial_value;
    
    for (int i = 0; i < n; i++) {
        void* arg = va_arg(args, void*);
        result = opr(result, arg);
    }
    
    va_end(args);
    
    return result;
}

int main() {
    int intArray[5] = {1, 2, 3, 4, 5};

    printf("Integer Array:\n");
    
    int initial_int = 0;
    int* intResult = (int*)vaggregate(add_op, 5, &initial_int, intArray, intArray + 1, intArray + 2, intArray + 3, intArray + 4);
    printf("Sum: %d\n", *intResult);

    initial_int = 1;
    intResult = (int*)vaggregate(mul_op, 5, &initial_int, intArray, intArray + 1, intArray + 2, intArray + 3, intArray + 4);
    printf("Product: %d\n", *intResult);

    initial_int = INT_MIN;
    intResult = (int*)vaggregate(max_op, 5, &initial_int, intArray, intArray + 1, intArray + 2, intArray + 3, intArray + 4);
    printf("Max: %d\n", *intResult);

    free(intResult);

    return 0;
}
