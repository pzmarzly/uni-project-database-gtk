#pragma once
#include "Repo.h"

void rm_test();
void rm_demo();

Timestamp timestamp(int year, int month, int day, unsigned char hour, unsigned char minute);
HourAndMinutes h(unsigned char hour);
HourAndMinutes hm(unsigned char hour, unsigned char minute);

void add_equipment(Repo *repo, EquipmentType type, char *name,
                   char *description);
void add_periodic(Repo *repo, Day day, HourAndMinutes start, HourAndMinutes end,
                  Timestamp active_since, Timestamp active_until,
                  char *description);
