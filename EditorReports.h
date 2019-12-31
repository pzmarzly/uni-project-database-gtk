#pragma once
#include <gtk/gtk.h>
#include "Repo.h"

typedef struct EditorReports EditorReports;
EditorReports* editor_reports_new(Repo *repo, GtkBuilder *ui);
void editor_reports_show(EditorReports *eq);
