#pragma once
#include "Repo.h"
#include <gtk/gtk.h>

Timestamp timestamp_now();
Timestamp timestamp_today();

GtkWidget *equipment_icon(EquipmentType type, unsigned size);
char *removal_window_title(TableID type);
char *editing_window_title(TableID type);

char *day_str(Day day);
char *hm_str(HourAndMinutes hm);
HourAndMinutes hm_parse(const char *str);
char *timestamp_day_str(Timestamp timestamp);
char *describe_periodic_reservation(PeriodicReservation r);
