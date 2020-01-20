#pragma once
#include "Repo.h"
#include "dialog/Dialogs.h"

PreparedEditDialog editor_edit_dialog_prepare(TableID type);
bool editor_removal_dialog(TableID type, char *element_name);
void validation_error(char *text);

bool ask_for_semester_dates(Timestamp *start, Timestamp *end);
bool ask_for_item_periodic(PeriodicReservation *r, ID r_id, Repo *repo);
