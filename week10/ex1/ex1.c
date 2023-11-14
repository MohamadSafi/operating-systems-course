#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <limits.h>

void find_all_hlinks(const char *source, const char *path) {
    DIR *dir;
    struct dirent *entry;
    struct stat source_stat, entry_stat;

    if (stat(source, &source_stat) == -1) {
        perror("Error getting stat of source file");
        return;
    }

    if (!(dir = opendir(path))) {
        perror("Error opening directory");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        char fullpath[PATH_MAX];
        snprintf(fullpath, PATH_MAX, "%s/%s", path, entry->d_name);

        if (stat(fullpath, &entry_stat) == -1) {
            perror("Error getting stat of file");
            continue;
        }

        if (entry_stat.st_ino == source_stat.st_ino) {
            char realpath_buf[PATH_MAX];
            realpath(fullpath, realpath_buf);
            printf("Hard link: %s (Inode: %lu)\n", realpath_buf, entry_stat.st_ino);
        }
    }

    closedir(dir);
}

void unlink_all(const char *source, const char *path) {
    DIR *dir;
    struct dirent *entry;
    struct stat source_stat, entry_stat;

    if (stat(source, &source_stat) == -1) {
        perror("Error getting stat of source file");
        return;
    }

    if (!(dir = opendir(path))) {
        perror("Error opening directory");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        char fullpath[PATH_MAX];
        snprintf(fullpath, PATH_MAX, "%s/%s", path, entry->d_name);

        if (stat(fullpath, &entry_stat) == -1) {
            perror("Error getting stat of file");
            continue;
        }

        if (entry_stat.st_ino == source_stat.st_ino && strcmp(fullpath, source) != 0) {
            unlink(fullpath);
            printf("Unlinked: %s\n", fullpath);
        }
    }

    closedir(dir);
}

void create_sym_link(const char *source, const char *link) {
    if (symlink(source, link) == -1) {
        perror("Error creating symbolic link");
    } else {
        printf("Symbolic link created: %s -> %s\n", link, source);
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <path_to_directory>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char source_file[PATH_MAX];
    char link1[PATH_MAX];
    char link2[PATH_MAX];
    char sym_link[PATH_MAX];
    char tmp_file[PATH_MAX];

    snprintf(source_file, PATH_MAX, "%s/myfile1.txt", argv[1]);
    snprintf(link1, PATH_MAX, "%s/myfile11.txt", argv[1]);
    snprintf(link2, PATH_MAX, "%s/myfile12.txt", argv[1]);
    snprintf(sym_link, PATH_MAX, "%s/myfile13.txt", argv[1]);
    snprintf(tmp_file, PATH_MAX, "/tmp/myfile1.txt");

    // Create a file and hard links
    FILE *file = fopen(source_file, "w");
    if (!file) {
        perror("Error creating file");
        return EXIT_FAILURE;
    }
    fprintf(file, "Hello world.\n");
    fclose(file);
    link(source_file, link1);
    link(source_file, link2);

    find_all_hlinks(source_file, argv[1]);

    rename(source_file, tmp_file);

    file = fopen(link1, "a");
    if (!file) {
        perror("Error opening file for modification");
        return EXIT_FAILURE;
    }
    fprintf(file, "Additional content.\n");
    fclose(file);

    create_sym_link(tmp_file, sym_link);

    unlink_all(link1, argv[1]);

    return 0;
}
