#include "Repo.h"
#include "RepoGenerate.h"
#include "RepoString.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void generate_demo() {
  rm_demo();
  Timestamp start = timestamp(2020, 1, 1, 0, 0);
  Timestamp end = timestamp(2021, 1, 1, 0, 0);

  Repo *r = repo_open("./demo.db", true, start, end);

  add_equipment(r, Laptop, "Laptop HP", "Opis laptopa HP.");
  add_equipment(r, Projector, "Rzutnik HP", "Opis rzutnika HP.");
  add_equipment(r, Projector, "Rzutnik Acer", "Opis rzutnika Acer.");
  add_equipment(r, Laptop, "Laptop Acer", "Opis laptopa Acer.");
  add_equipment(r, Laptop, "Laptop Lenovo", "Opis laptopa Lenovo.");
  add_equipment(r, Projector, "Ekran 51'", "Opis ekranu 51'.");
  add_equipment(r, Other, "Żółty pisak", "Opis żółtego pisaka.");

  add_periodic(r, Monday, h(9), h(12), start, end, "Rezerwacja poniedziałek 9:00-12:00.");
  add_periodic(r, Wednesday, h(12), h(14), start, end, "Rezerwacja środa 12:00-14:00.");

  repo_close(r);
}

int main() {
  generate_demo();
  return 0;
}
