#pragma once
#include "LinkedList.h"
#include "Repo.h"

Timestamp last_occurrence_before_time_range(PeriodicReservation *per,
                                            Timestamp start);
void periodic_generate_within_time_range(PeriodicReservation *per,
                                         Timestamp start, Timestamp end,
                                         LinkedList *ot_list);
int reservations_for_time_period(Repo *repo, Timestamp start, Timestamp end,
                                 OneTimeReservation **ot_list_destination,
                                 ID eq_id);
bool periodic_conflicts_with_periodic(PeriodicReservation *per1,
                                      PeriodicReservation *per2);
bool one_time_conflicts_with_periodic(PeriodicReservation *per,
                                      OneTimeReservation *ot);

bool periodic_can_have_equipment_attached(Repo *repo, PeriodicReservation *per,
                                          ID per_id, ID eq_id);
bool one_time_can_have_equipment_attached(Repo *repo, OneTimeReservation *ot,
                                          ID ot_id, ID eq_id);
