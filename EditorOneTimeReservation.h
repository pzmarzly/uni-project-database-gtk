#pragma once
#include "Editor.h"
#include "Repo.h"
#include <gtk/gtk.h>

typedef struct EditorOneTimeReservation EditorOneTimeReservation;
EditorOneTimeReservation *
editor_one_time_reservation_new(Editor *editor, Repo *repo, GtkBuilder *ui);
void editor_one_time_reservation_repopulate(EditorOneTimeReservation *this);
