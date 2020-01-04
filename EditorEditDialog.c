#include "EditorEditDialog.h"
#include "RepoData.h"
#include "Utils.h"

PreparedEditDialog editor_edit_dialog_prepare(TableID type) {
  GtkBuilder *ui = get_builder("EditorEditDialog.glade");
  GObject *dialog = gtk_builder_get_object(ui, "dialog");
  gtk_dialog_add_buttons(GTK_DIALOG(dialog), "Anuluj", GTK_RESPONSE_CANCEL,
                         "Zapisz", GTK_RESPONSE_OK, NULL);

  gtk_window_set_title(GTK_WINDOW(dialog), editing_window_title(type));

  PreparedEditDialog ret = {
      .ui = ui,
      .dialog = GTK_DIALOG(dialog),
  };
  return ret;
}
