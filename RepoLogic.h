#pragma once
#include "Repo.h"

bool periodic_slot_is_available(Repo *repo, PeriodicReservation *per, ID per_id,
                                ID eq_id);
bool one_time_is_available(Repo *repo, OneTimeReservation *ot, ID ot_id,
                           ID eq_id);
