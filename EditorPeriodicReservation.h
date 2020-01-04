#pragma once
#include "Repo.h"
#include <gtk/gtk.h>

typedef struct EditorPeriodicReservation EditorPeriodicReservation;
EditorPeriodicReservation *editor_periodic_reservation_new(Repo *repo, GtkBuilder *ui);
void editor_periodic_reservation_show(EditorPeriodicReservation *this);
