#pragma once
#define MAX_RECENT 5
int recent_load(char **dest);
void recent_save(char **src, int len);
int recent_push(char **src, int len, char *path);
int recent_del(char **src, int len, char *path);
