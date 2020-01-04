#include "Repo.h"
#include "RepoString.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void clean() { system("rm -f demo.db || true"); }

void equipment(Repo *repo, EquipmentType type, char *name, char *description) {
  ID id = repo_len(repo, TableEquipment);
  ID string_id = repo_string_len(repo);
  repo_string_set(repo, string_id, &description);
  Equipment eq = {
      .name = {0},
      .type = type,
      .description = string_id,
  };
  strcpy(eq.name, name);
  repo_set(repo, TableEquipment, id, &eq);
}

void generate_demo() {
  clean();
  Repo *r = repo_open("./demo.db", true, 0, 10000);

  equipment(r, Laptop, "Laptop HP", "Opis laptopa HP.");
  equipment(r, Projector, "Rzutnik HP", "Opis rzutnika HP.");
  equipment(r, Projector, "Rzutnik Acer", "Opis rzutnika Acer.");
  equipment(r, Laptop, "Laptop Acer", "Opis laptopa Acer.");
  equipment(r, Laptop, "Laptop Lenovo", "Opis laptopa Lenovo.");
  equipment(r, Projector, "Ekran 51'", "Opis ekranu 51'.");
  equipment(r, Other, "Żółty pisak", "Opis żółtego pisaka.");

  repo_close(r);
}

int main() {
  generate_demo();
  return 0;
}
