#pragma once
#include "Editor.h"
#include "Repo.h"
#include <gtk/gtk.h>

typedef struct EditorSemester EditorSemester;
EditorSemester *editor_semester_new(Repo *repo, GtkBuilder *ui, Editor *editor);
void editor_semester_show(EditorSemester *this);
