#include "RepoGenerate.h"
#include "Repo.h"
#include "RepoData.h"
#include "RepoString.h"
#include <stdlib.h>
#include <string.h>

void rm_test() { system("rm -f test.db || true"); }
void rm_demo() { system("rm -f demo.db || true"); }

Timestamp timestamp(int year, int month, int day, unsigned char hour) {
  GTimeZone *tz_utc = g_time_zone_new_utc();
  GDateTime *utc_time = g_date_time_new(tz_utc, year, month, day, hour, 0, 0);
  Timestamp ret = g_date_time_to_unix(utc_time);
  g_date_time_unref(utc_time);
  return ret;
}

void add_equipment(Repo *repo, EquipmentType type, char *name,
                   char *description) {
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

void add_periodic(Repo *repo, Day day, Hour start, Hour end,
                  Timestamp active_since, Timestamp active_until,
                  char *description) {
  ID id = repo_len(repo, TablePeriodicReservation);
  ID string_id = repo_string_len(repo);
  repo_string_set(repo, string_id, &description);
  PeriodicReservation r = {
      .item = 0,
      .day = day,
      .start = start,
      .end = end,
      .active_since = active_since,
      .active_until = active_until,
      .description = string_id,
  };
  repo_set(repo, TablePeriodicReservation, id, &r);
}
