#pragma once
#include "Repo.h"

void rm_test();
void rm_demo();

Timestamp timestamp(int year, int month, int day, unsigned char hm);
void add_equipment(Repo *repo, EquipmentType type, char *name,
                   char *description);
void add_periodic(Repo *repo, Day day, HourAndMinutes start, HourAndMinutes end,
                  Timestamp active_since, Timestamp active_until,
                  char *description);
