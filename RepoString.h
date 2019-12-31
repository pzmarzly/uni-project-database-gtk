#include "Repo.h"

bool repo_string_load(Repo *repo, String str, char **dest);
String repo_string_save(Repo *repo, char **src);
void repo_string_remove(Repo *repo, String str);
