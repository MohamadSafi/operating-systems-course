#include <stdio.h>
#include <string.h>

int main()
{
    char input[256];
    char ch;
    int length = 0;

    printf("Enter a text: ");

    while (1)
    {
        ch = getchar();

        if (ch == '.' || ch == '\n')
        {

            break;
        }

        if (length < sizeof(input) - 1)
        {
            input[length] = ch;
            length++;
        }
        else
        {

            return 1;
        }
    }

    input[length] = '\0';

    printf("Reversed string: \"");
    for (int i = length - 1; i >= 0; i--)
    {
        putchar(input[i]);
    }
    printf("\"\n");

    return 0;
}
