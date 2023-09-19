#include <stdio.h>

void convert(long long x, int s, int t) {
    if (s < 2 || s > 10 || t < 2 || t > 10) {
        printf("cannot convert!\n");
        return;
    }

    long long decimalValue = 0;
    long long multiplier = 1;
    while (x > 0) {
        int digit = x % 10;
        if (digit >= s) {
            printf("cannot convert!\n");
            return;
        }
        decimalValue += digit * multiplier;
        multiplier *= s;
        x /= 10;
    }

    char result[100]; 
    int index = 0;
    while (decimalValue > 0) {
        int digit = decimalValue % t;
        result[index++] = digit + '0'; 
        decimalValue /= t;
    }

    if (index == 0) {
        result[index++] = '0'; 
    }

    for (int i = index - 1; i >= 0; i--) {
        printf("%c", result[i]);
    }
    printf("\n");
}

int main() {
    long long num;
    int source, target;

    printf("Enter a number: ");
    if (scanf("%lld", &num) != 1 || num < 0) {
        printf("Invalid input!\n");
        return 1;
    }

    printf("Enter the source numeral system: ");
    if (scanf("%d", &source) != 1) {
        printf("Invalid input!\n");
        return 1;
    }

    printf("Enter the target numeral system: ");
    if (scanf("%d", &target) != 1) {
        printf("Invalid input!\n");
        return 1;
    }

    convert(num, source, target);

    return 0;
}
