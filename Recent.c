#include "Recent.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <gtk/gtk.h>

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

Recent* recent_load() {
    Recent *r = malloc(sizeof(Recent));
    FILE *fp = open_recent_file("rb");
    if (fp == NULL) goto fail;

    int items = fgetc(fp);
    if (items == EOF) goto fail;
    if (items > MAX_RECENT) items = MAX_RECENT;

    for (int i = 0; i < items; i++) {
        int len;
        if (fread(&len, sizeof(int), 1, fp) != 1) goto fail;
        r->paths[i] = malloc(len + 1);
        if (fread(r->paths[i], len + 1, 1, fp) != 1) goto fail;
    }

    fclose(fp);
    r->items = items;
    return r;

    fail:
    r->items = 0;
    return r;
}

void recent_close(Recent *r) {
    for (int i = 0; i < r->items; i++)
        free(r->paths[i]);
    free(r);
}

static void recent_save(Recent *r) {
    FILE *fp = open_recent_file("wb");
    if (fp == NULL) return;

    if (r->items > 255) r->items = 255;
    char items = r->items;
    fputc(items, fp);

    for (int i = 0; i < items; i++) {
        int len = strlen(r->paths[i]);
        fwrite(&len, sizeof(int), 1, fp);
        fwrite(r->paths[i], len + 1, 1, fp);
    }

    fclose(fp);
}

void recent_push(Recent *r, char *path) {
    char *new_element = g_strdup(path);

    // Remove duplicates.
    recent_del_all_equal(r, new_element);

    // Shift element right.
    for (int i = MAX_RECENT - 1; i > 0; i--)
        r->paths[i] = r->paths[i - 1];
    r->paths[0] = new_element;
    if (r->items < MAX_RECENT) r->items++;
    recent_save(r);
}

void recent_del_all_equal(Recent *r, char *path) {
    for (int i = 0; i < r->items; i++) {
        if (strcmp(r->paths[i], path) == 0) {
            free(r->paths[i]);
            if (i != r->items - 1) {
                // Shift elements left.
                for (int j = i + 1; j < r->items; j++) {
                    r->paths[j - 1] = r->paths[j];
                }
                // We have to check the element
                // that has just moved into paths[i].
                i--;
            }
            r->items--;
        }
    }
}
