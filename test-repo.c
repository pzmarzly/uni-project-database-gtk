#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "Repo.h"

void clean() {
    system("rm test-repo.db || true");
}

void creates_and_overrides() {
    clean();
    Repo *r = repo_open("./test-repo.db", true, 1);
    assert(r != NULL);
    assert(repo_get_semester_start(r) == 1);
    repo_close(r);
    r = repo_open("./test-repo.db", false, 2);
    assert(r != NULL);
    assert(repo_get_semester_start(r) == 1);
    repo_close(r);
    r = repo_open("./test-repo.db", true, 3);
    assert(r != NULL);
    assert(repo_get_semester_start(r) == 3);
    repo_close(r);
}

void handles_equipment() {
    clean();
    Repo *r = repo_open("./test-repo.db", true, 1);
    Equipment eq = {0, {0}, 0};
    assert(repo_len(r, TableEquipment) == 0);
    assert(repo_get(r, TableEquipment, 0, &eq) == false);
    repo_set(r, TableEquipment, 0, &eq);
    assert(repo_len(r, TableEquipment) == 1);
    eq.type = 1;
    assert(repo_get(r, TableEquipment, 0, &eq) == true);
    assert(eq.type == 0);
}

int main() {
    creates_and_overrides();
    handles_equipment();

    printf("OK\n");
    clean();
    return 0;
}
