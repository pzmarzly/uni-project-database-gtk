#pragma once
#include <stdbool.h>

typedef struct Editor Editor;
Editor* editor_new(char *path, bool overwrite);
void editor_set_quit_on_destroy(Editor *this, bool quit_on_destroy);
bool editor_run(Editor *this);
