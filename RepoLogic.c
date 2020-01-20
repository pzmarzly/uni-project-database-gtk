#include "RepoLogic.h"
#include "RepoData.h"

bool periodic_is_active_within_time_period(PeriodicReservation *res,
                                        Timestamp start, Timestamp end) {
  if (res->active_until <= start)
    return false;
  if (res->active_since >= end)
    return false;
  return true;
}

OneTimeReservation *reservations_for_time_period(Repo *repo, Timestamp start,
                                                 Timestamp end) {
  ID max = repo_len(repo, TablePeriodicReservation);
  for (ID i = 0; i < max; i++) {
    PeriodicReservation res;
    repo_get(repo, TablePeriodicReservation, i, &res);
    if (periodic_is_active_within_time_period(&res, start, end)) {
      // TODO: generate OneTimeReservation
    }
  }
  return NULL;
}

bool periodic_are_conflicting(PeriodicReservation *res1,
                            ID eq_res1_candidate, PeriodicReservation *res2) {
  if (res2->item != eq_res1_candidate)
    return false;
  if (res2->active_until <= res1->active_since)
    return false;
  if (res1->active_until <= res2->active_since)
    return false;
  if (res2->day != res1->day)
    return false;
  if (res1->end <= res2->start)
    return false;
  if (res2->end <= res1->start)
    return false;
  return true;
}

bool available_periodic_slot(Repo *repo, PeriodicReservation *res, ID res_id,
                             ID eq_id) {
  ID max = repo_len(repo, TablePeriodicReservation);
  for (ID i = 0; i < max; i++) {
    if (i == res_id)
      continue;
    PeriodicReservation res2;
    repo_get(repo, TablePeriodicReservation, i, &res2);
    if (periodic_are_conflicting(res, eq_id, &res2))
      return false;
  }
  return true;
}
