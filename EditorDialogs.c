#include "EditorDialogs.h"
#include "Datepicker.h"
#include "RepoData.h"
#include "RepoLogic.h"
#include <stdlib.h>

PreparedEditDialog editor_edit_dialog_prepare(TableID type) {
  return dialog_edit(editing_window_title(type));
}

bool editor_removal_dialog(TableID type, char *element_name) {
  return dialog_remove(removal_window_title(type), element_name);
}

void validation_error(char *text) {
  return dialog_info("Błąd walidacji", text);
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
      datepicker_new(GTK_BUTTON(start_btn), timestamp_today());
  gtk_grid_attach(grid, start_btn, 1, 0, 1, 1);
  gtk_widget_set_hexpand(start_btn, true);

  GtkWidget *end_btn = gtk_button_new_with_label("");
  Datepicker *end_dp = datepicker_new(GTK_BUTTON(end_btn), timestamp_today());
  gtk_grid_attach(grid, end_btn, 1, 1, 1, 1);
  gtk_widget_set_hexpand(end_btn, true);

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

bool ask_for_week_start(Timestamp *week_start) {
  PreparedEditDialog d = dialog_edit("Wybierz tydzień");
  GtkBuilder *ui = d.ui;
  GtkDialog *dialog = d.dialog;
  GtkGrid *grid = GTK_GRID(gtk_builder_get_object(ui, "grid"));

  GtkWidget *week_start_label =
      gtk_label_new("Wybierz dowolny dzień w danym tygodniu.");
  gtk_grid_attach(grid, week_start_label, 0, 0, 1, 1);

  GtkWidget *week_start_btn = gtk_button_new_with_label("");
  Datepicker *week_start_dp =
      datepicker_new(GTK_BUTTON(week_start_btn), timestamp_today());
  gtk_grid_attach(grid, week_start_btn, 0, 1, 1, 1);
  gtk_widget_set_hexpand(week_start_btn, true);

  gtk_widget_show_all(GTK_WIDGET(dialog));

  int result = gtk_dialog_run(dialog);
  *week_start = datepicker_read(week_start_dp);
  *week_start = timestamp_week_start(*week_start);
  gtk_widget_destroy(GTK_WIDGET(dialog));
  return result == GTK_RESPONSE_OK;
}

bool ask_for_moment(Timestamp *moment) {
  PreparedEditDialog d = dialog_edit("Wybierz moment");
  GtkBuilder *ui = d.ui;
  GtkDialog *dialog = d.dialog;
  GtkGrid *grid = GTK_GRID(gtk_builder_get_object(ui, "grid"));

  GtkWidget *day_label = gtk_label_new("Dzień: ");
  gtk_grid_attach(grid, day_label, 0, 0, 1, 1);
  GtkWidget *hm_label = gtk_label_new("Czas [HH:MM]: ");
  gtk_grid_attach(grid, hm_label, 0, 1, 1, 1);

  GtkWidget *day_btn = gtk_button_new_with_label("");
  Datepicker *day_dp = datepicker_new(GTK_BUTTON(day_btn), timestamp_today());
  gtk_grid_attach(grid, day_btn, 1, 0, 1, 1);

  GtkEntry *hm_entry = GTK_ENTRY(gtk_entry_new());
  gtk_entry_set_text(hm_entry, hm_str(timestamp_to_hm(timestamp_now())));
  gtk_entry_set_max_length(hm_entry, 5);
  gtk_grid_attach(grid, GTK_WIDGET(hm_entry), 1, 1, 1, 1);
  gtk_widget_set_hexpand(GTK_WIDGET(hm_entry), true);

  gtk_widget_show_all(GTK_WIDGET(dialog));

  while (true) {
    int result = gtk_dialog_run(dialog);
    if (result != GTK_RESPONSE_OK) {
      gtk_widget_destroy(GTK_WIDGET(dialog));
      return false;
    }
    const char *hm_str = gtk_entry_get_text(hm_entry);
    HourAndMinutes hm = hm_parse(hm_str);
    if (hm == HM_INVALID) {
      validation_error("Niepoprawny format czasu");
      continue;
    }
    *moment = hm_to_timestamp(datepicker_read(day_dp), hm);
    gtk_widget_destroy(GTK_WIDGET(dialog));
    return true;
  }
}

bool ask_for_item_periodic(PeriodicReservation *per, ID per_id, Repo *repo) {
  per->item = INVALID_ID;

  PreparedEditDialog d = dialog_edit("Wybierz przedmiot");
  GtkBuilder *ui = d.ui;
  GtkDialog *dialog = d.dialog;
  GtkGrid *grid = GTK_GRID(gtk_builder_get_object(ui, "grid"));

  GtkWidget *item_label = gtk_label_new("Przedmiot: ");
  gtk_grid_attach(grid, item_label, 0, 0, 1, 1);

  GtkComboBox *item_combo_box = GTK_COMBO_BOX(gtk_combo_box_text_new());

  ID eq_len = repo_len(repo, TableEquipment);
  ID *mappings = malloc(eq_len * sizeof(ID));
  ID mapping_len = 0;
  for (ID i = 0; i < eq_len; i++) {
    if (!periodic_can_have_equipment_attached(repo, per, per_id, i))
      continue;

    mappings[mapping_len++] = i;

    Equipment eq;
    repo_get(repo, TableEquipment, i, &eq);
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(item_combo_box), NULL,
                              eq.name);
  }
  gtk_combo_box_set_active(GTK_COMBO_BOX(item_combo_box), per->item);
  gtk_grid_attach(grid, GTK_WIDGET(item_combo_box), 0, 1, 1, 1);
  gtk_widget_set_hexpand(GTK_WIDGET(item_combo_box), true);

  gtk_widget_show_all(GTK_WIDGET(dialog));

  int item;
  while (true) {
    int result = gtk_dialog_run(dialog);
    if (result != GTK_RESPONSE_OK) {
      gtk_widget_destroy(GTK_WIDGET(dialog));
      return false;
    }
    item = gtk_combo_box_get_active(item_combo_box);
    if (item >= 0 && (ID)item < mapping_len)
      break;
  }

  per->item = mappings[item];
  gtk_widget_destroy(GTK_WIDGET(dialog));
  return true;
}

bool ask_for_item_one_time(OneTimeReservation *ot, ID ot_id, Repo *repo) {
  (void)ot_id; // unused, but kept for consistency with ask_for_item_periodic
  ot->item = INVALID_ID;

  PreparedEditDialog d = dialog_edit("Wybierz przedmiot");
  GtkBuilder *ui = d.ui;
  GtkDialog *dialog = d.dialog;
  GtkGrid *grid = GTK_GRID(gtk_builder_get_object(ui, "grid"));

  GtkWidget *item_label = gtk_label_new("Przedmiot: ");
  gtk_grid_attach(grid, item_label, 0, 0, 1, 1);

  GtkComboBox *item_combo_box = GTK_COMBO_BOX(gtk_combo_box_text_new());

  ID eq_len = repo_len(repo, TableEquipment);
  ID *mappings = malloc(eq_len * sizeof(ID));
  ID mapping_len = 0;
  for (ID i = 0; i < eq_len; i++) {
    if (!one_time_can_have_equipment_attached(repo, ot, i))
      continue;

    mappings[mapping_len++] = i;

    Equipment eq;
    repo_get(repo, TableEquipment, i, &eq);
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(item_combo_box), NULL,
                              eq.name);
  }
  gtk_combo_box_set_active(GTK_COMBO_BOX(item_combo_box), ot->item);
  gtk_grid_attach(grid, GTK_WIDGET(item_combo_box), 0, 1, 1, 1);
  gtk_widget_set_hexpand(GTK_WIDGET(item_combo_box), true);

  gtk_widget_show_all(GTK_WIDGET(dialog));

  int item;
  while (true) {
    int result = gtk_dialog_run(dialog);
    if (result != GTK_RESPONSE_OK) {
      gtk_widget_destroy(GTK_WIDGET(dialog));
      return false;
    }
    item = gtk_combo_box_get_active(item_combo_box);
    if (item >= 0 && (ID)item < mapping_len)
      break;
  }

  ot->item = mappings[item];
  gtk_widget_destroy(GTK_WIDGET(dialog));
  return true;
}
