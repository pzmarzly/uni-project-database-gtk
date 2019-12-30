#include <stdbool.h>

typedef struct Repo Repo;
Repo* repo_open(char *path, bool create);
void repo_close(Repo *repo);
