#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    DIR *dir;               // Pointer to a directory stream
    struct dirent *entry;   // Pointer to a directory entry structure

    // Attempt to open the root directory
    dir = opendir("/");
    if (dir == NULL) {
        perror("opendir");  // If opening directory failed, print the error message
        return EXIT_FAILURE;// Exit with a failure status
    }
    // Read entries from the directory stream
    while ((entry = readdir(dir)) != NULL) {
        printf("%s\n", entry->d_name);  // Print the name of each directory entry
    }

    closedir(dir);          // Close the directory stream
    return EXIT_SUCCESS;    // Exit with a success status
}
