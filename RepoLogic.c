#include "RepoLogic.h"
#include "RepoData.h"

bool periodic_slot_conflict(Repo *repo, PeriodicReservation *res, ID res2_id, ID eq_id) {
  PeriodicReservation r;
  repo_get(repo, TablePeriodicReservation, res2_id, &r);
  PeriodicReservation *res2 = &r;

  if (res2->item != eq_id) return false;
  if (res2->active_until <= res->active_since) return false;
  if (res->active_until <= res2->active_since) return false;
  if (res2->day != res->day) return false;
  if (res->end <= res2->start) return false;
  if (res2->end <= res->start) return false;
  return true;
}

bool available_periodic_slot(Repo *repo, PeriodicReservation *res, ID res_id,
                             ID eq_id) {
  ID max = repo_len(repo, TablePeriodicReservation);
  for (ID i = 0; i < max; i++) {
    if (i == res_id) continue;
    if (periodic_slot_conflict(repo, res, i, eq_id)) return false;
  }
  return true;
}
