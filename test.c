#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "Repo.h"
#include "RepoString.h"

void clean() {
    system("rm test-repo.db || true");
}

void opens_and_overwrites() {
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

void handles_data() {
    clean();
    Repo *r = repo_open("./test-repo.db", true, 1);
    Equipment eq = {1, {0}, 3};
    repo_set(r, TableEquipment, 0, &eq);
    StringFragment str = {3, 5, {0}};

    assert(repo_len(r, TableStringFragment) == 0);
    assert(repo_get(r, TableStringFragment, 0, &str) == false);
    repo_set(r, TableStringFragment, 0, &str);
    assert(repo_len(r, TableStringFragment) == 1);

    str.len = 1;
    assert(repo_get(r, TableStringFragment, 0, &str) == true);
    assert(str.len == 5);
    repo_close(r);

    r = repo_open("./test-repo.db", false, 1);
    assert(repo_len(r, TableStringFragment) == 1);
    str.len = 1;
    assert(repo_get(r, TableStringFragment, 0, &str) == true);
    assert(str.len == 5);

    repo_del(r, TableStringFragment, 0);
    assert(repo_len(r, TableStringFragment) == 0);
    assert(repo_get(r, TableStringFragment, 0, &str) == false);

    assert(repo_len(r, TableEquipment) == 1);
    eq.description = 5;
    assert(repo_get(r, TableEquipment, 0, &eq) == true);
    assert(eq.description == 3);
    repo_close(r);
}

void handles_strings() {
    clean();
    Repo *r = repo_open("./test-repo.db", true, 1);
    char *c1 = "Trochę tekstu.", *c2 = "Drugi tekst.", *c3 = "Trzeci tekst.";
    char *str = c1;

    assert(repo_string_load(r, 0, &str) == false);
    assert(repo_string_save(r, &str) == 0);
    str = NULL;
    assert(repo_string_load(r, 0, &str) == true);
    assert(strcmp(str, c1) == 0);
    free(str);

    str = c2;
    assert(repo_string_save(r, &str) == 1);
    repo_string_remove(r, 1);
    str = c3;
    assert(repo_string_save(r, &str) == 2);

    str = NULL;
    assert(repo_string_load(r, 0, &str) == true);
    assert(strcmp(str, c1) == 0);
    free(str);
    str = NULL;
    assert(repo_string_load(r, 1, &str) == false);
    assert(repo_string_load(r, 2, &str) == true);
    assert(strcmp(str, c3) == 0);
    free(str);

    repo_string_remove(r, 0);
    str = NULL;
    assert(repo_string_load(r, 0, &str) == false);
    assert(repo_string_load(r, 1, &str) == false);
    assert(repo_string_load(r, 2, &str) == true);
    assert(strcmp(str, c3) == 0);
    free(str);

    repo_close(r);
}

int main() {
    opens_and_overwrites();
    handles_data();
    handles_strings();

    printf("OK\n");
    clean();
    return 0;
}
