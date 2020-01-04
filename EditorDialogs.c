#include "EditorDialogs.h"
#include "RepoData.h"
#include "Datepicker.h"

PreparedEditDialog editor_edit_dialog_prepare(TableID type) {
  return dialog_edit(editing_window_title(type));
}

bool editor_removal_dialog(TableID type, char *element_name) {
  return dialog_remove(removal_window_title(type), element_name);
}

bool ask_for_semester_dates(Timestamp *start, Timestamp *end) {
  PreparedEditDialog d = dialog_edit("Wybierz datę");
  GtkBuilder *ui = d.ui;
  GtkDialog *dialog = d.dialog;
  GtkGrid *grid = GTK_GRID(gtk_builder_get_object(ui, "grid"));

  GtkWidget *start_label = gtk_label_new("Początek semestru: ");
  gtk_grid_attach(grid, start_label, 0, 0, 1, 1);
  GtkWidget *end_label = gtk_label_new("Koniec semestru: ");
  gtk_grid_attach(grid, end_label, 0, 1, 1, 1);
  GtkWidget *info_label =
      gtk_label_new("Semestr musi trwać co najmniej 28 dni.");
  gtk_grid_attach(grid, info_label, 0, 2, 2, 1);

  GtkWidget *start_btn = gtk_button_new_with_label("");
  Datepicker *start_dp =
      datepicker_new(GTK_BUTTON(start_btn), timestamp_today(), NULL, NULL);
  gtk_grid_attach(grid, start_btn, 1, 0, 1, 1);

  GtkWidget *end_btn = gtk_button_new_with_label("");
  Datepicker *end_dp =
      datepicker_new(GTK_BUTTON(end_btn), timestamp_today(), NULL, NULL);
  gtk_grid_attach(grid, end_btn, 1, 1, 1, 1);

  gtk_widget_show_all(GTK_WIDGET(dialog));

  do {
    int result = gtk_dialog_run(dialog);
    if (result != GTK_RESPONSE_OK)
      return false;
    *start = datepicker_read(start_dp);
    *end = datepicker_read(end_dp);
  } while (*start + 3600 * 24 * 28 >= *end);
  gtk_widget_destroy(GTK_WIDGET(dialog));
  return true;
}
