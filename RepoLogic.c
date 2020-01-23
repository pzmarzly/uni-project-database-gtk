#include "RepoLogic.h"
#include "LinkedList.h"
#include "RepoData.h"

bool periodic_active_is_within_time_range(PeriodicReservation *per,
                                          Timestamp start, Timestamp end) {
  if (per->active_until <= start)
    return false;
  if (per->active_since >= end)
    return false;
  return true;
}

bool one_time_is_within_time_range(OneTimeReservation *ot, Timestamp start,
                                   Timestamp end) {
  if (ot->end <= start)
    return false;
  if (ot->start >= end)
    return false;
  return true;
}

void periodic_generate_within_time_range(PeriodicReservation *per,
                                         Timestamp start, Timestamp end,
                                         LinkedList *ot_list_destination) {
  if (!periodic_active_is_within_time_range(per, start, end))
    return;
  Timestamp last_occurrence_before = start;
  if (timestamp_to_day(last_occurrence_before) == per->day) {
  }
  // while (per->end >= last_occurrence_before)

  OneTimeReservation *ot = malloc(sizeof(OneTimeReservation));
  ot->type = Reservation;
  ot->item = per->item;
  // ot->start = start;
  // ot->end = end;
  ot->description = per->description;
  linked_list_add(ot_list_destination, ot);
}

/// You can filter the reservation by item ID by providing eq_id other than
/// INVALID_ID.
int reservations_for_time_period(Repo *repo, Timestamp start, Timestamp end,
                                 OneTimeReservation **ot_list_destination,
                                 ID eq_id) {
  ID max = repo_len(repo, TablePeriodicReservation);
  LinkedList *list = linked_list_new();
  for (ID i = 0; i < max; i++) {
    PeriodicReservation per;
    repo_get(repo, TablePeriodicReservation, i, &per);
    if (eq_id != INVALID_ID && eq_id != per.item)
      continue;
    periodic_generate_within_time_range(&per, start, end, list);
  }
  return linked_list_into_array(list, sizeof(OneTimeReservation),
                                (void **)ot_list_destination);
}

bool periodic_conflicts_with_periodic(PeriodicReservation *per1,
                                      PeriodicReservation *per2) {
  if (per1->item != per2->item)
    return false;
  if (per1->day != per2->day)
    return false;
  if (per1->end <= per2->start)
    return false;
  if (per2->end <= per1->start)
    return false;
  return true;
}

bool one_time_conflicts_with_periodic(PeriodicReservation *per,
                                      OneTimeReservation *ot) {
  if (ot->item != per->item)
    return false;
  // TODO: logic
  return true;
}

bool periodic_slot_is_available(Repo *repo, PeriodicReservation *per, ID per_id,
                                ID eq_id) {
  PeriodicReservation per1;
  memcpy(&per1, per, sizeof(PeriodicReservation));
  per1.item = eq_id;

  ID periodic_max = repo_len(repo, TablePeriodicReservation);
  for (ID i = 0; i < periodic_max; i++) {
    if (i == per_id)
      continue;

    PeriodicReservation per2;
    repo_get(repo, TablePeriodicReservation, i, &per2);
    if (!periodic_active_is_within_time_range(&per2, per1.active_since,
                                              per1.active_until))
      continue;

    if (periodic_conflicts_with_periodic(&per1, &per2))
      return false;
  }

  ID one_time_max = repo_len(repo, TableOneTimeReservation);
  for (ID i = 0; i < one_time_max; i++) {
    OneTimeReservation ot;
    repo_get(repo, TableOneTimeReservation, i, &ot);
    if (!one_time_is_within_time_range(&ot, per1.active_since,
                                       per1.active_until))
      continue;

    if (one_time_conflicts_with_periodic(&per1, &ot))
      return false;
  }
  return true;
}

bool one_time_is_available(Repo *repo, OneTimeReservation *ot, ID ot_id,
                           ID eq_id) {
  return true; // TODO: logic
}
