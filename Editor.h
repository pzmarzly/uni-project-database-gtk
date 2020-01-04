#pragma once
#include "Repo.h"
#include <stdbool.h>

typedef struct Editor Editor;
Editor *editor_new(char *path, bool overwrite, Timestamp start, Timestamp end);
void editor_set_quit_on_destroy(Editor *this, bool quit_on_destroy);
bool editor_start(Editor *this);
