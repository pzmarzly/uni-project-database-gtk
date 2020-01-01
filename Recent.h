#pragma once
#define MAX_RECENT 5
typedef struct {
    char *paths[MAX_RECENT];
    int items;
} Recent;
Recent* recent_load();
void recent_close(Recent *r);
void recent_push(Recent *r, char *path);
void recent_del(Recent *r, char *path);
