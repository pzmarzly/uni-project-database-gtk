#pragma once
#include "Repo.h"
#include <gtk/gtk.h>

typedef struct EditorSemester EditorSemester;
EditorSemester *editor_semester_new(Repo *repo, GtkBuilder *ui);
void editor_semester_show(EditorSemester *this);
