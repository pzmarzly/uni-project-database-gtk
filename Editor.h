#pragma once
#include "Repo.h"
#include <stdbool.h>

typedef struct Editor Editor;
Editor *editor_new(char *path, RepoType repo_type);
void editor_set_quit_on_destroy(Editor *this, bool quit_on_destroy);
bool editor_start(Editor *this);
void editor_refresh(Editor *this);
