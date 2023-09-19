#include <stdio.h>
#include <string.h>
#include <ctype.h>

int count(const char *str, char target) {
    int count = 0;
    
    while (*str) {
        if (tolower(*str) == tolower(target)) {
            count++;
        }
        str++;
    }
    
    return count;
}

void countAll(const char *str) {
    int length = strlen(str);
    int first = 1; 

    for (int i = 0; i < length; i++) {
        char c = str[i];
        if (isalpha(c)) {
            int charCount = count(str, c);
            if (!first) {
                printf(", ");
            }
            printf("%c:%d", tolower(c), charCount);
            first = 0; 
        }
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <input_string>\n", argv[0]);
        return 1;
    }

    char *input = argv[1];

    countAll(input);

    return 0;
}
