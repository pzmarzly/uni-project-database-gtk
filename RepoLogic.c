#include "RepoLogic.h"
#include "RepoData.h"
#include "LinkedList.h"

bool periodic_active_is_within_time_range(PeriodicReservation *res,
                                          Timestamp start, Timestamp end) {
  if (res->active_until <= start)
    return false;
  if (res->active_since >= end)
    return false;
  return true;
}

bool one_time_is_within_time_range(OneTimeReservation *res, Timestamp start,
                                   Timestamp end) {
  if (res->end <= start)
    return false;
  if (res->start >= end)
    return false;
  return true;
}

int reservations_for_time_period(Repo *repo, Timestamp start, Timestamp end,
                                 OneTimeReservation **output) {
  ID max = repo_len(repo, TablePeriodicReservation);
  LinkedList* list = linked_list_new();
  // for (ID i = 0; i < max; i++) {
  //   PeriodicReservation res;
  //   repo_get(repo, TablePeriodicReservation, i, &res);
  //   if (periodic_active_is_within_time_range(&res, start, end))
  //     output_len++;
  // }
  // OneTimeReservation *arr = malloc(output_len * sizeof(OneTimeReservation));
  // int arr_id = 0;
  // for (ID i = 0; i < max; i++) {
  //   PeriodicReservation res;
  //   repo_get(repo, TablePeriodicReservation, i, &res);
  //   if (periodic_active_is_within_time_range(&res, start, end)) {
  //     // TODO: generate OneTimeReservation
  //     arr[arr_id].item = res.item;
  //     arr[arr_id].type = Reservation;
  //     arr[arr_id].start = Reservation;
  //     arr[arr_id].end = Reservation;
  //     arr[arr_id].description = res.description;
  //     arr_id++;
  //   }
  // }
  return linked_list_into_array(list, sizeof(OneTimeReservation), (void **)output);
}

bool periodic_conflicts_with_periodic(PeriodicReservation *res1,
                                      ID eq_res1_candidate,
                                      PeriodicReservation *res2) {
  if (res2->item != eq_res1_candidate)
    return false;
  if (res2->day != res1->day)
    return false;
  if (res1->end <= res2->start)
    return false;
  if (res2->end <= res1->start)
    return false;
  return true;
}

bool one_time_conflicts_with_periodic(PeriodicReservation *res,
                                      ID eq_res_candidate,
                                      OneTimeReservation *ot) {
  if (ot->item != eq_res_candidate)
    return false;
  // TODO: logic
  return true;
}

bool periodic_slot_is_available(Repo *repo, PeriodicReservation *res, ID res_id,
                                ID eq_id) {
  ID periodic_max = repo_len(repo, TablePeriodicReservation);
  for (ID i = 0; i < periodic_max; i++) {
    if (i == res_id)
      continue;
    PeriodicReservation res2;
    repo_get(repo, TablePeriodicReservation, i, &res2);
    if (!periodic_active_is_within_time_range(&res2, res->active_since,
                                              res->active_until))
      continue;
    if (periodic_conflicts_with_periodic(res, eq_id, &res2))
      return false;
  }
  ID one_time_max = repo_len(repo, TableOneTimeReservation);
  for (ID i = 0; i < one_time_max; i++) {
    OneTimeReservation ot;
    repo_get(repo, TableOneTimeReservation, i, &ot);
    if (!one_time_is_within_time_range(&ot, res->active_since,
                                       res->active_until))
      continue;
    if (one_time_conflicts_with_periodic(res, eq_id, &ot))
      return false;
  }
  return true;
}

bool one_time_is_available(Repo *repo, OneTimeReservation *res, ID res_id,
                           ID eq_id) {
  return true; // TODO: logic
}
