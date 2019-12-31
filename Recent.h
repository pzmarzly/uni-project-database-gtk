#pragma once
int recent_load(char **dest, int max);
void recent_save(char **src, int len);
int recent_push(char **src, int len, int limit, char *path);
