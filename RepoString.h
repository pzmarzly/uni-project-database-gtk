#pragma once
#include "Repo.h"

bool repo_string_get(Repo *repo, ID id, char **dest);
void repo_string_set(Repo *repo, ID id, char **src);
void repo_string_del(Repo *repo, ID id);
ID repo_string_len(Repo *repo);
