#include "Dialogs.h"
#include <gtk/gtk.h>
#include "../Utils.h"

void dialog_info(char *text) {
  GtkBuilder *ui = get_builder("dialog/Info.glade");
  GObject *dialog = gtk_builder_get_object(ui, "dialog");
  gtk_dialog_add_buttons(GTK_DIALOG(dialog), "OK", GTK_RESPONSE_YES, NULL);

  GObject *label = gtk_builder_get_object(ui, "label");
  gtk_label_set_text(GTK_LABEL(label), text);

  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(GTK_WIDGET(dialog));
}
