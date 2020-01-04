#pragma once
#include "Repo.h"
#include <gtk/gtk.h>

typedef struct {
  GtkBuilder *ui;
  GtkDialog *dialog;
} PreparedEditDialog;
PreparedEditDialog editor_edit_dialog_prepare(TableID type);
