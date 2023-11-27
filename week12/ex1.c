#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>
#include <string.h>

#define PE_COMBO (1 << 0) // Bitmask for P+E
#define CAP_COMBO (1 << 1) // Bitmask for C+A+P
#define MS_COMBO (1 << 2) // Bitmask for M+S

// Function to create a bitmask based on current key states
int createKeyCombo(int keys[]) {
    int combo = 0;

    if (keys[KEY_P] && keys[KEY_E]) combo |= PE_COMBO;
    if (keys[KEY_C] && keys[KEY_A] && keys[KEY_P]) combo |= CAP_COMBO;
    if (keys[KEY_M] && keys[KEY_S]) combo |= MS_COMBO;

    return combo;
}

// Function to check and print custom shortcut messages
void checkShortcuts(int keys[], FILE* outputFile) {
    int combo = createKeyCombo(keys);

    switch (combo) {
        case PE_COMBO: // In case that P + E are pressed .
            printf("I passed the Exam!\n");
            fprintf(outputFile, "I passed the Exam!\n");
            break;
        case CAP_COMBO: // In case that C + A + P pressed.
            printf("Get some cappuccino!\n");
            fprintf(outputFile, "Get some cappuccino!\n");
            break;
        case MS_COMBO: // In case that M + S are pressed 
            printf("Mohammad Safi\n");
            fprintf(outputFile, "Mohammad Safi\n");
            break;
    }
}
int main() {
    const char *deviceFile = "/dev/input/by-path/platform-i8042-serio-0-event-kbd"; // Defining the path to the keyboard device file.
    struct input_event event; // Declaring a structure to store individual input events.
    int fd, rd, size = sizeof(struct input_event);
    char *eventValue[3] = {"RELEASED", "PRESSED ", "REPEATED"}; // Array to represent the states of a key event.
    int keys[KEY_MAX] = {0}; // This Array for keeping track of key states

    // Opening Keyboard Device
    fd = open(deviceFile, O_RDONLY);
    if (fd == -1) {
        printf("Failed to open device try running the program with sudo!\n");
        exit(1);
    }

    // Opening the file to save the output
    FILE *outputFile = fopen("ex1.txt", "w");
    if (outputFile == NULL) {
        printf("Failed to open the output file.\n");
        exit(1);
    }

    // Print available shortcuts
    printf("Availble shortcuts:\n");
    printf("The Custom shortcut is M+S.\n");
    printf("P+E → 'I passedthe Exam!'.\n");
    printf("C+A+P → 'Get some cappuccino!'.\n");
    printf("E+X → terminate the program.\n");


    // The Main loop for endlessly reading from the file to get all events
    while (1) {
        rd = read(fd, &event, size);
        if (rd < size) {
            perror("read()");
            exit(1);
        }

        // Checking for the key events (Press, Release, Repeat)
        if (event.type == EV_KEY && (event.value >= 0 && event.value <= 2)) {
            printf("%s 0x%04x (%d)\n", eventValue[event.value], event.code, event.code);
            fprintf(outputFile, "%s 0x%04x (%d)\n", eventValue[event.value], event.code, event.code);
            fflush(outputFile);  // Flushing after writing to file to ensure data is written to file immediately

            keys[event.code] = event.value; // Updating key state

            checkShortcuts(keys, outputFile); // Using the function checkShortcuts to check for any shortcuts pressed

            // Terminate if 'E' and 'X' are pressed
            if (keys[KEY_E] && keys[KEY_X]) {
                printf("Exiting program.\n");
                break;
            }
        }
    }

    // Closing device and file
    close(fd);
    fclose(outputFile);

    return 0;
}
