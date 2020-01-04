#pragma once
#include "Repo.h"
#include <gtk/gtk.h>

typedef struct EditorReports EditorReports;
EditorReports *editor_reports_new(Repo *repo, GtkBuilder *ui);
void editor_reports_show(EditorReports *this);
