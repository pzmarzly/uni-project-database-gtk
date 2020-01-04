#pragma once
#include "Repo.h"
#include <gtk/gtk.h>

GtkWidget *equipment_icon(EquipmentType type, unsigned size);
char *removal_and_editing_text(TableID type);

char *day_str(Day day);
char *hm_str(HourAndMinutes hm);
HourAndMinutes hm_parse(const char *str);
char *timestamp_day_str(Timestamp timestamp);
char *describe_periodic_reservation(PeriodicReservation r);
