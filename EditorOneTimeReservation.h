#pragma once
#include "Repo.h"
#include <gtk/gtk.h>

typedef struct EditorOneTimeReservation EditorOneTimeReservation;
EditorOneTimeReservation *editor_one_time_reservation_new(Repo *repo,
                                                           GtkBuilder *ui);
void editor_one_time_reservation_show(EditorOneTimeReservation *this);
