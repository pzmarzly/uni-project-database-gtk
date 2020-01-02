#pragma once
#define MAX_RECENT 5
typedef struct {
    char *paths[MAX_RECENT];
    int items;
} RecentList;
RecentList* recent_list_load();
void recent_list_close(RecentList *r);
void recent_list_push(RecentList *r, char *path);
void recent_list_del_all_equal(RecentList *r, char *path);
