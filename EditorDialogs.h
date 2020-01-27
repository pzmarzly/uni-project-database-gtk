#pragma once
#include "Repo.h"
#include "dialog/Dialogs.h"

PreparedEditDialog editor_edit_dialog_prepare(TableID type);
bool editor_removal_dialog(TableID type, char *element_name);
void validation_error(char *text);

bool ask_for_semester_dates(Timestamp *start, Timestamp *end);
bool ask_for_week_start(Timestamp *week_start);
bool ask_for_moment(Timestamp *moment);

bool ask_for_item_periodic(PeriodicReservation *per, ID per_id, Repo *repo);
bool ask_for_item_one_time(OneTimeReservation *ot, ID ot_id, Repo *repo);
