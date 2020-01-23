#pragma once
#include "Repo.h"

bool periodic_can_have_equipment_attached(Repo *repo, PeriodicReservation *per,
                                          ID per_id, ID eq_id);
bool one_time_can_have_equipment_attached(Repo *repo, OneTimeReservation *ot,
                                          ID ot_id, ID eq_id);
