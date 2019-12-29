#include <stdlib.h>

const char* PROGRAM_FOLDER = ".local/share/pl.pzmarzly.management";
const char* RECENT_FILE = "recent.bin";

void create_program_folder() {
    int path_len = strlen(getenv("HOME")) + 1 + strlen(PROGRAM_FOLDER);
    char *path = malloc(path_len + 1);
    strcpy(path, getenv("HOME"));
    strcat(path, "/");
    strcat(path, PROGRAM_FOLDER);
    if (mkdir(path, 0700) == -1) {
        if (errno != EEXIST) {
            printf("failed to create directory %s, code %d\n", path, errno);
            return NULL;
        }
    }
    free(path);
}

FILE *get_recent_file(const char *mode) {
    create_program_folder();

    int path_len = dir_path_len + 1 + strlen(RECENT_FILE);
    char *path = malloc(path_len + 1);
    strcpy(path, getenv("HOME"));
    strcat(path, "/");
    strcat(path, dir);
    strcat(path, "/");
    strcat(path, RECENT_FILE);
    FILE *fp = fopen(path, mode);
    free(path);

    return fp;
}

int repo_recent_load(char **dest) {
    FILE *fp = get_recent_file("rb");
    if (fp == NULL) return 0;

    int items = fgetc(fp);
    if (items == EOF) return 0;
    if (items > 5) items = 5;

    for (int i = 0; i < items; i++) {
        int len;
        if (fread(&len, sizeof(int), 1, fp) != 1) return 0;
        dest[i] = malloc(len + 1);
        if (fread(dest[i], len + 1, 1, fp) != 1) return 0;
    }

    fclose(fp);
    return items;
}

void repo_recent_save(char **src, int items) {
    FILE *fp = get_recent_file("wb");
    if (fp == NULL) return;

    if (items > 5) items = 5;
    char items_c = items;
    fputc(items_c, fp);

    for (int i = 0; i < items; i++) {
        int len = strlen(src[i]);
        fwrite(len, sizeof(int), 1, fp);
        fwrite(src[i], len + 1, 1, fp);
    }

    fclose(fp);
}
