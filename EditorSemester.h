#pragma once
#include <gtk/gtk.h>
#include "Repo.h"

typedef struct EditorSemester EditorSemester;
EditorSemester* editor_semester_new(Repo *repo, GtkBuilder *ui);
void editor_semester_show(EditorSemester *this);
