#pragma once
#include "Editor.h"
#include "Repo.h"
#include <gtk/gtk.h>

typedef struct EditorReports EditorReports;
EditorReports *editor_reports_new(Editor *editor, Repo *repo, GtkBuilder *ui);
void editor_reports_repopulate(EditorReports *this);
