#pragma once
#include "Repo.h"
#include <gtk/gtk.h>

Timestamp timestamp_now();
Timestamp timestamp_midnight(Timestamp timestamp);
Timestamp timestamp_week_start(Timestamp timestamp);
Timestamp timestamp_today();
Timestamp timestamp_add_week(Timestamp timestamp);

Day timestamp_to_day(Timestamp timestamp);
HourAndMinutes timestamp_to_hm(Timestamp timestamp);
Timestamp hm_to_timestamp(Timestamp midnight, HourAndMinutes hm);

GtkWidget *equipment_icon(EquipmentType type, unsigned size);
char *removal_window_title(TableID type);
char *editing_window_title(TableID type);

char *day_str(Day day);
char *hm_str(HourAndMinutes hm);
HourAndMinutes hm_parse(const char *str);
char *timestamp_day_str(Timestamp timestamp);

char *describe_periodic_reservation(Repo *repo, PeriodicReservation *r);
char *describe_one_time_reservation(Repo *repo, OneTimeReservation *r);
