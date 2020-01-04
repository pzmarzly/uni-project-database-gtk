#include "EditorSemester.h"
#include "EditorDialogs.h"
#include "RecentList.h"
#include "Repo.h"
#include "RepoData.h"
#include "Utils.h"
#include <gtk/gtk.h>
#include <stdbool.h>

struct EditorSemester {
  Repo *repo;
  GtkBuilder *ui;
  GtkWindow *parent;
};

EditorSemester *editor_semester_new(Repo *repo, GtkBuilder *ui) {
  EditorSemester *this = malloc(sizeof(EditorSemester));
  this->repo = repo;
  this->ui = ui;
  this->parent = GTK_WINDOW(gtk_builder_get_object(ui, "window"));
  return this;
}

static void update_starting_date_btn(GtkButton *button, Repo *repo) {
  char *date = timestamp_day_str(repo_get_semester_start(repo));
  gtk_button_set_label(button, date);
  free(date);
}

static void on_starting_date(GtkWidget *sender, gpointer user_data) {
  EditorSemester *this = (EditorSemester *)user_data;

  PreparedEditDialog d = editor_edit_dialog_prepare(TableEquipment);
  GtkBuilder *ui = d.ui;
  GtkDialog *dialog = d.dialog;
  GtkGrid *grid = GTK_GRID(gtk_builder_get_object(ui, "grid"));

  GtkWidget *calendar_label =
      GTK_WIDGET(gtk_label_new("Data rozpoczęcia semestru:"));
  gtk_grid_attach(grid, calendar_label, 0, 0, 1, 1);

  GDateTime *time =
      g_date_time_new_from_unix_utc(repo_get_semester_start(this->repo));
  if (time == NULL) {
    printf("Niepoprawna data rozpoczęcia semestru\n");
    return;
  }
  int year, month, day;
  g_date_time_get_ymd(time, &year, &month, &day);

  GtkCalendar *calendar = GTK_CALENDAR(gtk_calendar_new());
  gtk_calendar_select_month(calendar, month - 1, year);
  gtk_calendar_select_day(calendar, day);
  g_date_time_unref(time);
  gtk_grid_attach(grid, GTK_WIDGET(calendar), 0, 1, 1, 1);
  gtk_widget_set_hexpand(GTK_WIDGET(calendar), true);
  gtk_widget_set_vexpand(GTK_WIDGET(calendar), true);

  gtk_widget_show_all(GTK_WIDGET(dialog));

  int result = gtk_dialog_run(dialog);
  if (result == GTK_RESPONSE_OK) {
    gtk_calendar_get_date(calendar, (unsigned *)&year, (unsigned *)&month,
                          (unsigned *)&day);
    GTimeZone *tz = g_time_zone_new_utc();
    time = g_date_time_new(tz, year, month + 1, day, 0, 0, 0);

    repo_set_semester_start(this->repo, g_date_time_to_unix(time));
    update_starting_date_btn(GTK_BUTTON(sender), this->repo);
  }
  gtk_widget_destroy(GTK_WIDGET(dialog));
}

static void on_save_as(GtkWidget *sender, gpointer user_data) {
  (void)sender;
  EditorSemester *this = (EditorSemester *)user_data;

  GtkWidget *dialog = gtk_file_chooser_dialog_new(
      "Zapisz bazę", GTK_WINDOW(this->parent), GTK_FILE_CHOOSER_ACTION_SAVE,
      "Anuluj", GTK_RESPONSE_CANCEL, "Zapisz", GTK_RESPONSE_ACCEPT, NULL);
  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), file_filter());
  gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog),
                                                 TRUE);
  gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), "kopia.db");

  int res = gtk_dialog_run(GTK_DIALOG(dialog));
  if (res == GTK_RESPONSE_ACCEPT) {
    char *path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
    char *old_path = repo_get_path(this->repo);
    if (strcmp(path, old_path) == 0) {
      gtk_widget_destroy(dialog);
      free(old_path);
      free(path);
      return;
    }
    Repo *old_repo = repo_save_as(this->repo, path);
    repo_close(old_repo);

    char title[8192];
    strcpy(title, path);
    strcat(title, " - WeźMnie");
    gtk_window_set_title(this->parent, title);

    RecentList *recent_list = recent_list_load();
    recent_list_push(recent_list, path);
    free(old_path);
    free(path);
  }
  gtk_widget_destroy(dialog);
}

static void update_active_btn(GtkButton *button, bool active) {
  gtk_button_set_label(button,
                       active ? "Zatrzymaj semestr" : "Rozpocznij semestr");
}

void on_active(GtkWidget *sender, gpointer user_data) {
  EditorSemester *this = (EditorSemester *)user_data;
  bool active = repo_get_semester_active(this->repo);
  // TODO: dialog
  active = !active;
  repo_set_semester_active(this->repo, active);
  update_active_btn(GTK_BUTTON(sender), active);
}

void editor_semester_show(EditorSemester *this) {
  GObject *starting_date_btn =
      gtk_builder_get_object(this->ui, "semester-starting-date");
  update_starting_date_btn(GTK_BUTTON(starting_date_btn), this->repo);
  g_signal_connect(G_OBJECT(starting_date_btn), "clicked",
                   G_CALLBACK(on_starting_date), this);

  GObject *active_btn = gtk_builder_get_object(this->ui, "semester-active");
  bool active = repo_get_semester_active(this->repo);
  update_active_btn(GTK_BUTTON(active_btn), active);
  g_signal_connect(G_OBJECT(active_btn), "clicked", G_CALLBACK(on_active),
                   this);

  GObject *save_as_btn = gtk_builder_get_object(this->ui, "semester-save-as");
  g_signal_connect(G_OBJECT(save_as_btn), "clicked", G_CALLBACK(on_save_as),
                   this);
}
