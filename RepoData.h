#pragma once
#include "Repo.h"
#include <gtk/gtk.h>

Timestamp timestamp_now();
Timestamp timestamp_midnight(Timestamp timestamp);
Timestamp timestamp_today();

Day timestamp_to_day(Timestamp timestamp);
Hour timestamp_to_hour(Timestamp timestamp);
Timestamp hour_to_timestamp(Timestamp midnight, Hour hour);

GtkWidget *equipment_icon(EquipmentType type, unsigned size);
char *removal_window_title(TableID type);
char *editing_window_title(TableID type);

char *day_str(Day day);
char *hour_str(Hour hour);
Hour hour_parse(const char *str);
char *timestamp_day_str(Timestamp timestamp);

char *describe_periodic_reservation(Repo *repo, PeriodicReservation *r);
char *describe_one_time_reservation(Repo *repo, OneTimeReservation *r);
