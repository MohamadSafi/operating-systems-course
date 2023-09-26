#include <stdio.h>
#include <string.h>

#define MAX_NAME 64
#define MAX_PATH 2048
#define MAX_DATA 1024
#define MAX_FILES 100

struct File {
    unsigned int id;
    char name[MAX_NAME];
    size_t size;
    char data[MAX_DATA];
    struct Directory* directory;
};


struct Directory {
    char name[MAX_NAME];
    struct File files[MAX_FILES];
    struct Directory* sub_dir[MAX_FILES];
    unsigned char nf;
    unsigned char nd;
    char path[MAX_PATH];
};


void overwrite_to_file(struct File* file, const char* str) {
    strcpy(file->data, str);
    file->size = strlen(str) + 1;
}


void append_to_file(struct File* file, const char* str) {
    strcat(file->data, str);
    file->size = strlen(file->data) + 1;
}


void printp_file(struct File* file) {
    printf("%s", file->directory->path);
    printf("/%s\n", file->name);
}


void show_file(struct File* file) {
    printf("%s ", file->name);
}


void show_dir(struct Directory* dir) {
    printf("\nDIRECTORY\n");
    printf("path: %s\n", dir->path);
    printf("files:\n [ ");
    for (int i = 0; i < dir->nf; i++) {
        show_file(&(dir->files[i]));
    }
    printf("]\n");
    printf("directories:\n { ");
    for (int i = 0; i < dir->nd; i++) {
        show_dir(dir->sub_dir[i]);
    }
    printf("}\n");
}


void add_file(struct File* file, struct Directory* dir) {
    if (dir->nf < MAX_FILES) {
        dir->files[dir->nf] = *file;
        file->directory = dir;
        dir->nf++;
    } else {
        printf("Directory is full. Cannot add more files.\n");
    }
}


void add_dir(struct Directory* sub_dir, struct Directory* dir) {
    if (dir->nd < MAX_FILES) {
        dir->sub_dir[dir->nd] = sub_dir;
        dir->nd++;
        char temp_path[MAX_PATH];
        if (strcmp(dir->path, "/")) {
            strcpy(temp_path, dir->path);
            strcat(temp_path, "/");
            strcat(temp_path, sub_dir->name);
            strcpy(sub_dir->path, temp_path);
        } else {
            strcpy(temp_path, "/");
            strcat(temp_path, sub_dir->name);
            strcpy(sub_dir->path, temp_path);
        }
    } else {
        printf("Directory is full. Cannot add more sub_directories.\n");
    }
}

int main() {

    struct Directory root;
    strcpy(root.name, "/");
    root.nf = 0;
    root.nd = 0;
    strcpy(root.path, "/");

    struct Directory home;
    strcpy(home.name, "home");
    home.nf = 0;
    home.nd = 0;
    add_dir(&home, &root);

    struct Directory bin;
    strcpy(bin.name, "bin");
    bin.nf = 0;
    bin.nd = 0;
    add_dir(&bin, &root);


    struct File bash;
    bash.id = 1;
    strcpy(bash.name, "bash");
    overwrite_to_file(&bash, "Bourne Again Shell!!");
    add_file(&bash, &bin);

    struct File ex3_1;
    ex3_1.id = 2;
    strcpy(ex3_1.name, "ex3_1.c");
    overwrite_to_file(&ex3_1, "int printf(const char * format, ...);");
    add_file(&ex3_1, &home);

    struct File ex3_2;
    ex3_2.id = 3;
    strcpy(ex3_2.name, "ex3_2.c");
    overwrite_to_file(&ex3_2, "// This is a comment in C language");
    add_file(&ex3_2, &home);


    append_to_file(&ex3_1, "int main(){printf(\"Hello World!\");}");


    show_dir(&root);


    printf("File Paths:\n");
    printp_file(&bash);
    printp_file(&ex3_1);
    printp_file(&ex3_2);

    return 0;
}
