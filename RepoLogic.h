#pragma once
#include "Repo.h"

bool periodic_slot_is_available(Repo *repo, PeriodicReservation *res, ID res_id,
                                ID eq_id);
