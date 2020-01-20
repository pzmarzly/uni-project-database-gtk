#include "Repo.h"
#include "RepoString.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void clean() { system("rm -f test-repo.db || true"); }

void opens_and_overwrites() {
  clean();
  Repo *r = repo_open("./test-repo.db", true, 1, 2);
  assert(r != NULL);
  assert(repo_get_semester_start(r) == 1);
  repo_close(r);
  r = repo_open("./test-repo.db", false, 0, 1);
  assert(r != NULL);
  assert(repo_get_semester_start(r) == 1);
  repo_close(r);
  r = repo_open("./test-repo.db", true, 0, 1);
  assert(r != NULL);
  assert(repo_get_semester_start(r) == 0);
  repo_close(r);
}

void handles_data() {
  clean();
  Repo *r = repo_open("./test-repo.db", true, 0, 1);
  Equipment eq = {1, {0}, 3};
  repo_set(r, TableEquipment, 0, &eq);
  StringMetadata str = {3, 5};

  assert(repo_len(r, TableStringMetadata) == 0);
  repo_get(r, TableStringMetadata, 0, &str);
  repo_set(r, TableStringMetadata, 0, &str);
  assert(repo_len(r, TableStringMetadata) == 1);

  str.len = 1;
  repo_get(r, TableStringMetadata, 0, &str);
  assert(str.len == 5);
  repo_close(r);

  r = repo_open("./test-repo.db", false, 0, 1);
  assert(repo_len(r, TableStringMetadata) == 1);
  str.len = 1;
  repo_get(r, TableStringMetadata, 0, &str);
  assert(str.len == 5);

  repo_del(r, TableStringMetadata, 0);
  assert(repo_len(r, TableStringMetadata) == 0);
  repo_get(r, TableStringMetadata, 0, &str);

  assert(repo_len(r, TableEquipment) == 1);
  eq.description = 5;
  repo_get(r, TableEquipment, 0, &eq);
  assert(eq.description == 3);
  repo_close(r);
}

void handles_strings() {
  clean();
  Repo *r = repo_open("./test-repo.db", true, 0, 1);
  char *c1 = "Troche tekstu.", *c2 = "Drugi tekst.", *c3 = "Trzeci tekst.";
  char *str = c1;

  repo_string_get(r, 0, &str);
  repo_string_set(r, 0, &str);
  str = NULL;
  repo_string_get(r, 0, &str);
  assert(strcmp(str, c1) == 0);
  free(str);

  str = c2;
  repo_string_set(r, 1, &str);
  repo_string_del(r, 1);
  str = c3;
  repo_string_set(r, 2, &str);

  str = NULL;
  repo_string_get(r, 0, &str);
  assert(strcmp(str, c1) == 0);
  free(str);
  str = NULL;
  repo_string_get(r, 1, &str);
  repo_string_get(r, 2, &str);
  assert(strcmp(str, c3) == 0);
  free(str);

  repo_string_del(r, 0);
  str = NULL;
  repo_string_get(r, 0, &str);
  repo_string_get(r, 1, &str);
  repo_string_get(r, 2, &str);
  assert(strcmp(str, c3) == 0);
  free(str);

  repo_close(r);
}

void handles_data_large() {
  clean();
  Repo *r = repo_open("./test-repo.db", true, 0, 1);
  Equipment eq = {1, {0}, 3};

  for (ID i = 0; i < 1000; i++) {
    eq.description = i;
    repo_set(r, TableEquipment, i, &eq);
  }
  assert(repo_len(r, TableEquipment) == 1000);

  // Delete every second one.
  for (ID i = 0, deleted = 0; i < 1000; i += 2, deleted++) {
    repo_del(r, TableEquipment, i - deleted);
  }
  assert(repo_len(r, TableEquipment) == 500);

  for (ID i = 0; i < 500; i++) {
    repo_get(r, TableEquipment, i, &eq);
    assert(eq.description == 2 * i + 1);
  }

  assert(repo_len(r, TableEquipment) == 500);
  repo_close(r);
  r = repo_open("./test-repo.db", false, 0, 1);
  assert(repo_len(r, TableEquipment) == 500);

  for (ID i = 0; i < 500; i++) {
    repo_get(r, TableEquipment, i, &eq);
    assert(eq.description == 2 * i + 1);
  }
  assert(repo_len(r, TableEquipment) == 500);

  repo_del_n(r, TableEquipment, 100, 100);
  assert(repo_len(r, TableEquipment) == 400);
  repo_close(r);
  r = repo_open("./test-repo.db", false, 0, 1);
  assert(repo_len(r, TableEquipment) == 400);
  repo_get(r, TableEquipment, 0, &eq);
  assert(eq.description == 1);
  repo_get(r, TableEquipment, 99, &eq);
  assert(eq.description == 2 * 99 + 1);
  repo_get(r, TableEquipment, 100, &eq);
  assert(eq.description == 2 * 200 + 1);
  repo_get(r, TableEquipment, 101, &eq);
  assert(eq.description == 2 * 201 + 1);
  repo_close(r);
}

int main() {
  opens_and_overwrites();
  handles_data();
  handles_strings();

  handles_data_large();

  printf("OK\n");
  clean();
  return 0;
}
