#include "AboutDialog.h"
#include "Utils.h"
#include <gtk/gtk.h>

void about_dialog() {
  GtkBuilder *ui = get_builder("AboutDialog.glade");
  GObject *dialog = gtk_builder_get_object(ui, "dialog");
  gtk_dialog_add_buttons(GTK_DIALOG(dialog), "OK", GTK_RESPONSE_YES, NULL);

  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(GTK_WIDGET(dialog));
}
