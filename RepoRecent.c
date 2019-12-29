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

    int i, j;
    for (i = 0; i < items; i++) {
        char len1 = fgetc(fp);
        if (len1 == EOF) return 0;
        char len2 = fgetc(fp);
        if (len2 == EOF) return 0;
        int len = len1 << 8 | len2;
        if (len > 8192) return 0;
        char *item = malloc(len + 1);
        for (j = 0; j < len; j++) {
            char c = fgetc(fp);
            if (c == EOF) return 0;
            item[j] = c;
        }
        item[j] = '\0';
        dest[i] = item;
    }

    fclose(fp);
    return i;
}

void repo_recent_save(char **src, int items) {
    FILE *fp = get_recent_file("wb");
    if (fp == NULL) return;

    if (items > 5) items = 5;
    char items_c = items;
    fputc(items_c, fp);

    for (int i = 0; i < items; i++) {
        int len = strlen(src[i]);
        if (len > 8192) continue;
        fputc(len >> 8 & 0xFF, fp);
        fputc(len & 0xFF, fp);
        for (int j = 0; j < len; j++) {
            fputc(item[j], fp);
        }
        fputc('\0', fp);
    }

    fclose(fp);
}
