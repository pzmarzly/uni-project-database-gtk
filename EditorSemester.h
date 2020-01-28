#pragma once
#include "Editor.h"
#include "Repo.h"
#include <gtk/gtk.h>

typedef struct EditorSemester EditorSemester;
EditorSemester *editor_semester_new(Editor *editor, Repo *repo, GtkBuilder *ui);
void editor_semester_prepare(EditorSemester *this);
void editor_semester_refresh(EditorSemester *this);
