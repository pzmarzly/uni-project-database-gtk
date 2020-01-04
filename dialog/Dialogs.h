#pragma once
#include <gtk/gtk.h>
#include <stdbool.h>

typedef struct {
  GtkBuilder *ui;
  GtkDialog *dialog;
} PreparedEditDialog;
void dialog_info(char *title, char *text);
PreparedEditDialog dialog_edit(char *title);
bool dialog_confirm(char *title, char *text);
bool dialog_remove(char *title, char *element_name);
