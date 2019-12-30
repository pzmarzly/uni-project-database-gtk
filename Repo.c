#include "Repo.h"
#include <stdlib.h>
#include <stdio.h>

struct Repo {
    FILE *file;
};

Repo* repo_open(char *path, bool create) {
    Repo *repo = malloc(sizeof(Repo));
    repo->file = fopen(path, create ? "wb" : "r+b");
    if (repo->file == NULL) return NULL;
    return repo;
}

void repo_close(Repo *repo) {
    if (repo != NULL) {
        free(repo);
    }
}
