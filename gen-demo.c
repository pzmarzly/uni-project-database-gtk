#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Repo.h"
#include "RepoString.h"

void clean() {
    system("rm -f demo.db || true");
}

void generate_demo() {
    clean();
    Repo *r = repo_open("./demo.db", true);

    char *c1 = "Trochę tekstu.";
    repo_string_set(r, 0, &c1);

    Equipment eq = {
        .name = "Rzutnik 1",
        .type = Projector,
        .description = 0,
    };
    repo_set(r, TableEquipment, 0, &eq);

    repo_close(r);
}

int main() {
    generate_demo();
    return 0;
}
