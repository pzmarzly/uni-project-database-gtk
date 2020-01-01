#include "Recent.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

const char* PROGRAM_FOLDER = ".local/share/pl.pzmarzly.management";
const char* RECENT_FILE = "recent.bin";

static void create_program_folder() {
    int path_len = strlen(getenv("HOME")) + 1
        + strlen(PROGRAM_FOLDER);
    char *path = malloc(path_len + 1);
    strcpy(path, getenv("HOME"));
    strcat(path, "/");
    strcat(path, PROGRAM_FOLDER);
    if (mkdir(path, 0700) == -1) {
        if (errno != EEXIST) {
            printf("failed to create directory %s, code %d\n", path, errno);
            return;
        }
    }
    free(path);
}

static FILE *open_recent_file(const char *mode) {
    create_program_folder();

    int path_len = strlen(getenv("HOME")) + 1
        + strlen(PROGRAM_FOLDER) + 1
        + strlen(RECENT_FILE);
    char *path = malloc(path_len + 1);
    strcpy(path, getenv("HOME"));
    strcat(path, "/");
    strcat(path, PROGRAM_FOLDER);
    strcat(path, "/");
    strcat(path, RECENT_FILE);
    FILE *fp = fopen(path, mode);
    free(path);

    return fp;
}

int recent_load(char **dest) {
    FILE *fp = open_recent_file("rb");
    if (fp == NULL) return 0;

    int len = fgetc(fp);
    if (len == EOF) return 0;
    if (len > MAX_RECENT) len = MAX_RECENT;

    for (int i = 0; i < len; i++) {
        int len;
        if (fread(&len, sizeof(int), 1, fp) != 1) return 0;
        dest[i] = malloc(len + 1);
        if (fread(dest[i], len + 1, 1, fp) != 1) return 0;
    }

    fclose(fp);
    return len;
}

void recent_save(char **src, int len) {
    FILE *fp = open_recent_file("wb");
    if (fp == NULL) return;

    if (len > 255) len = 255;
    char len_c = len;
    fputc(len_c, fp);

    for (int i = 0; i < len; i++) {
        int len = strlen(src[i]);
        fwrite(&len, sizeof(int), 1, fp);
        fwrite(src[i], len + 1, 1, fp);
    }

    fclose(fp);
}

int recent_push(char **src, int len, char *path) {
    // remove duplicates
    len = recent_del(src, len, path);

    // shift right
    for (int i = MAX_RECENT - 1; i > 0; i--)
        src[i] = src[i - 1];
    src[0] = path;
    if (len < MAX_RECENT) len++;
    recent_save(src, len);

    return len;
}

int recent_del(char **src, int len, char *path) {
    for (int i = 0; i < len; i++) {
        if (strcmp(src[i], path) == 0) {
            if (i != len - 1)
                src[i] = src[i + 1];
            len--;
        }
    }
    return len;
}
