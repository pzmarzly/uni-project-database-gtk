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

void periodic(Repo *repo, Day day, HourAndMinutes start, HourAndMinutes end,
              Timestamp active_since, Timestamp active_until,
              char *description) {
  ID id = repo_len(repo, TablePeriodicReservation);
  ID string_id = repo_string_len(repo);
  repo_string_set(repo, string_id, &description);
  PeriodicReservation r = {
      .day = day,
      .start = start,
      .end = end,
      .active_since = active_since,
      .active_until = active_until,
      .description = string_id,
  };
  repo_set(repo, TablePeriodicReservation, id, &r);
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

  periodic(r, Monday, 9 * 60, 12 * 60, 0, 10000,
           "Rezerwacja poniedziałek 9-12.");
  periodic(r, Wednesday, 12 * 60 + 15, 14 * 60, 0, 10000,
           "Rezerwacja środa 12:15-14:00.");

  repo_close(r);
}

int main() {
  generate_demo();
  return 0;
}
