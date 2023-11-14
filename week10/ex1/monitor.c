#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/inotify.h>

#define MAX_EVENTS 1024
#define LEN_NAME 1024
#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN (MAX_EVENTS * (EVENT_SIZE + LEN_NAME))

int fd, wd;

void print_stat_info(const char *path) {
    struct stat statbuf;
    if (stat(path, &statbuf) == -1) {
        perror("stat");
        return;
    }
    printf("Stat info - File: %s, Size: %ld, Inode: %ld\n", path, statbuf.st_size, statbuf.st_ino);
}

void list_directory(const char *path) {
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(path)))
        return;

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char fullpath[1024];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);
        print_stat_info(fullpath);
    }

    closedir(dir);
}

void signal_handler(int signum) {
    printf("Caught signal %d, exiting and printing stats.\n", signum);
    list_directory(".");
    inotify_rm_watch(fd, wd);
    close(fd);
    exit(0);
}

void monitor_directory(const char *path) {
    char buffer[BUF_LEN];

    while (1) {
        int length = read(fd, buffer, BUF_LEN);
        if (length < 0) {
            perror("read");
        }

        int i = 0;
        while (i < length) {
            struct inotify_event *event = (struct inotify_event *)&buffer[i];
            if (event->len) {
                char fullpath[1024];
                snprintf(fullpath, sizeof(fullpath), "%s/%s", path, event->name);

                if (event->mask & IN_CREATE) {
                    printf("The file %s was created.\n", fullpath);
                    print_stat_info(fullpath);
                }

                if (event->mask & IN_DELETE) {
                    printf("The file %s was deleted.\n", fullpath);
                }

                if (event->mask & IN_MODIFY) {
                    printf("The file %s was modified.\n", fullpath);
                    print_stat_info(fullpath);
                }

                if (event->mask & IN_ACCESS) {
                    printf("The file %s was accessed.\n", fullpath);
                }

                if (event->mask & IN_ATTRIB) {
                    printf("The metadata of %s was changed.\n", fullpath);
                    print_stat_info(fullpath);
                }

                if (event->mask & IN_OPEN) {
                    printf("The file %s was opened.\n", fullpath);
                }
            }
            i += EVENT_SIZE + event->len;
        }
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s <path_to_watch>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, signal_handler);

    fd = inotify_init();
    if (fd < 0) {
        perror("inotify_init");
        exit(EXIT_FAILURE);
    }

    wd = inotify_add_watch(fd, argv[1], IN_CREATE | IN_DELETE | IN_MODIFY | IN_ACCESS | IN_ATTRIB | IN_OPEN);
    if (wd == -1) {
        perror("inotify_add_watch");
        exit(EXIT_FAILURE);
    }

    printf("Monitoring %s\n", argv[1]);
    list_directory(argv[1]);
    monitor_directory(argv[1]);

    inotify_rm_watch(fd, wd);
    close(fd);

    return 0;
}
