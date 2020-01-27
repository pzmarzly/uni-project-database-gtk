#pragma once
#include "LinkedList.h"
#include "Repo.h"

bool periodic_is_active_within_time_range(PeriodicReservation *per,
                                          Timestamp start, Timestamp end);
bool one_time_is_within_time_range(OneTimeReservation *ot, Timestamp start,
                                   Timestamp end);

Timestamp last_occurrence_before_time_range(PeriodicReservation *per,
                                            Timestamp start);
void periodic_generate_within_time_range(PeriodicReservation *per,
                                         Timestamp start, Timestamp end,
                                         LinkedList *ot_list);
ID reservations_for_time_period(Repo *repo, Timestamp start, Timestamp end,
                                OneTimeReservation **ot_list_destination,
                                ID eq_id, ID skip_id);
bool periodic_conflicts_with_periodic(PeriodicReservation *per1,
                                      PeriodicReservation *per2);
bool one_time_conflicts_with_periodic(PeriodicReservation *per,
                                      OneTimeReservation *ot);

bool periodic_can_have_equipment_attached(Repo *repo, PeriodicReservation *per,
                                          ID per_id, ID eq_id);
bool one_time_can_have_equipment_attached(Repo *repo, OneTimeReservation *ot,
                                          ID ot_id, ID eq_id);
