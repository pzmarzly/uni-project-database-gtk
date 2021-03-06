#include "RepoGenerate.h"
#include "Repo.h"
#include "RepoData.h"
#include "RepoString.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

// If running `true` returns an error, we're dealing with an insane system.
void rm_test() { assert(system("rm -f test-repo.db || true") == 0); }
void rm_demo() { assert(system("rm -f demo.db || true") == 0); }

Timestamp timestamp(int year, int month, int day, unsigned char hour,
                    unsigned char minute) {
  GTimeZone *tz_local = g_time_zone_new_local();
  GDateTime *time =
      g_date_time_new(tz_local, year, month, day, hour, minute, 0);
  Timestamp ret = g_date_time_to_unix(time);
  g_date_time_unref(time);
  return ret;
}

HourAndMinutes h(unsigned char hour) { return hour * 60; }

HourAndMinutes hm(unsigned char hour, unsigned char minute) {
  return hour * 60 + minute;
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

void add_periodic(Repo *repo, Day day, HourAndMinutes start, HourAndMinutes end,
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
