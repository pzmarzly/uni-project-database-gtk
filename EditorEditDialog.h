#pragma once
#include <gtk/gtk.h>
#include "Repo.h"

typedef struct {
    GtkBuilder *ui;
    GtkDialog *dialog;
} PreparedEditDialog;
PreparedEditDialog editor_edit_dialog_prepare(TableID type);
