#pragma once
#include "Editor.h"
#include "Repo.h"
#include <gtk/gtk.h>

typedef struct EditorPeriodicReservation EditorPeriodicReservation;
EditorPeriodicReservation *
editor_periodic_reservation_new(Editor *editor, Repo *repo, GtkBuilder *ui);
void editor_periodic_reservation_repopulate(EditorPeriodicReservation *this);
