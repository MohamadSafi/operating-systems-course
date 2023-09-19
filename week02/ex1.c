#include <stdio.h>
#include <limits.h>
#include <float.h>

int main() {
    int max_int = INT_MAX;
    unsigned short max_unsigned_short = USHRT_MAX;
    long int max_long = LONG_MAX;
    float max_float = FLT_MAX;
    double max_double = DBL_MAX;

    printf("Size of int: %lu bytes\n", sizeof(int));
    printf("Maximum int value: %d\n", max_int);

    printf("Size of unsigned short: %lu bytes\n", sizeof(unsigned short));
    printf("Maximum unsigned short value: %u\n", max_unsigned_short);

    printf("Size of signed long int: %lu bytes\n", sizeof(long int));
    printf("Maximum signed long int value: %ld\n", max_long);

    printf("Size of float: %lu bytes\n", sizeof(float));
    printf("Maximum float value: %e\n", max_float);

    printf("Size of double: %lu bytes\n", sizeof(double));
    printf("Maximum double value: %e\n", max_double);

    return 0;
}
