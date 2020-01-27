#include "Dialogs.h"
#include "../Utils.h"
#include <stdlib.h>
#include <string.h>

void dialog_info(char *title, char *text) {
  GtkBuilder *ui = get_builder("dialog/Info.glade");
  GObject *dialog = gtk_builder_get_object(ui, "dialog");
  gtk_window_set_title(GTK_WINDOW(dialog), title);
  gtk_dialog_add_buttons(GTK_DIALOG(dialog), "OK", GTK_RESPONSE_YES, NULL);

  GObject *label = gtk_builder_get_object(ui, "label");
  gtk_label_set_text(GTK_LABEL(label), text);

  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(GTK_WIDGET(dialog));
}

PreparedEditDialog dialog_edit(char *title) {
  GtkBuilder *ui = get_builder("dialog/Edit.glade");
  GObject *dialog = gtk_builder_get_object(ui, "dialog");
  gtk_window_set_title(GTK_WINDOW(dialog), title);
  gtk_dialog_add_buttons(GTK_DIALOG(dialog), "Anuluj", GTK_RESPONSE_CANCEL,
                         "OK", GTK_RESPONSE_OK, NULL);

  PreparedEditDialog ret = {
      .ui = ui,
      .dialog = GTK_DIALOG(dialog),
  };
  return ret;
}

bool dialog_confirm(char *title, char *text) {
  GtkBuilder *ui = get_builder("dialog/Confirm.glade");
  GObject *dialog = gtk_builder_get_object(ui, "dialog");
  gtk_window_set_title(GTK_WINDOW(dialog), title);
  gtk_dialog_add_buttons(GTK_DIALOG(dialog), "Nie", GTK_RESPONSE_NO, "Tak",
                         GTK_RESPONSE_YES, NULL);

  GObject *label = gtk_builder_get_object(ui, "label");
  gtk_label_set_text(GTK_LABEL(label), text);

  int result = gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(GTK_WIDGET(dialog));
  return result == GTK_RESPONSE_YES;
}

bool dialog_remove(char *title, char *element_name) {
  char *text = malloc(strlen(element_name) + 64);
  strcpy(text, "Czy na pewno chcesz usunąć ");
  strcat(text, element_name);
  strcat(text, "?");
  bool res = dialog_confirm(title, text);
  free(text);
  return res;
}
