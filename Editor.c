// Paweł Zmarzły 314569
// Projekt, grupa MPi śr 12-14

#include "Editor.h"
#include "Datepicker.h"
#include "EditorEquipment.h"
#include "EditorReports.h"
#include "EditorSemester.h"
#include "RepoData.h"
#include "Utils.h"
#include "dialog/Dialogs.h"

struct Editor {
  GtkBuilder *ui;
  bool quit_on_destroy;
  GObject *window;
  Repo *repo;
  char *repo_path;

  EditorSemester *semester;
  EditorEquipment *equipment;
  EditorReports *reports;
};

typedef struct {
  Timestamp start;
  Timestamp end;
} Semester;

static Semester dummy_semester_dates() {
  Semester ret = {.start = 0, .end = 10000};
  return ret;
}

static Semester ask_for_semester_dates() {
  PreparedEditDialog d = dialog_edit("Wybierz datę");
  GtkBuilder *ui = d.ui;
  GtkDialog *dialog = d.dialog;
  GtkGrid *grid = GTK_GRID(gtk_builder_get_object(ui, "grid"));

  GtkWidget *start_label = gtk_label_new("Początek semestru: ");
  gtk_grid_attach(grid, start_label, 0, 0, 1, 1);
  GtkWidget *end_label = gtk_label_new("Koniec semestru: ");
  gtk_grid_attach(grid, end_label, 0, 1, 1, 1);
  GtkWidget *info_label = gtk_label_new("Semestr musi trwać co najmniej 28 dni.");
  gtk_grid_attach(grid, info_label, 0, 2, 2, 1);

  GtkWidget *start_btn = gtk_button_new_with_label("");
  Datepicker *start =
      datepicker_new(GTK_BUTTON(start_btn), timestamp_today(), NULL, NULL);
  gtk_grid_attach(grid, start_btn, 1, 0, 1, 1);

  GtkWidget *end_btn = gtk_button_new_with_label("");
  Datepicker *end =
      datepicker_new(GTK_BUTTON(end_btn), timestamp_today(), NULL, NULL);
  gtk_grid_attach(grid, end_btn, 1, 1, 1, 1);

  gtk_widget_show_all(GTK_WIDGET(dialog));

  Semester ret;
  do {
    int result = gtk_dialog_run(dialog);
    if (result != GTK_RESPONSE_OK)
      exit(0);
    ret.start = datepicker_read(start);
    ret.end = datepicker_read(end);
  } while (ret.start + 3600 * 24 * 28 >= ret.end);
  gtk_widget_destroy(GTK_WIDGET(dialog));
  return ret;
}

Editor *editor_new(char *path, RepoType repo_type) {
  Editor *this = malloc(sizeof(Editor));
  this->ui = get_builder("Editor.glade");
  this->quit_on_destroy = false;
  this->window = NULL;
  Semester s =
      repo_type == RepoNew ? ask_for_semester_dates() : dummy_semester_dates();
  this->repo = repo_open(path, repo_type, s.start, s.end);
  this->repo_path = g_strdup(path);
  this->semester = editor_semester_new(this->repo, this->ui);
  this->equipment = editor_equipment_new(this->repo, this->ui);
  this->reports = editor_reports_new(this->repo, this->ui);
  return this;
}

void editor_set_quit_on_destroy(Editor *this, bool quit_on_destroy) {
  this->quit_on_destroy = quit_on_destroy;
}

static void on_destroy(GtkWidget *sender, gpointer user_data) {
  (void)sender;
  Editor *this = (Editor *)user_data;
  if (this->quit_on_destroy)
    gtk_main_quit();
  repo_close(this->repo);
  free(this);
}

bool editor_start(Editor *this) {
  this->window = gtk_builder_get_object(this->ui, "window");
  g_signal_connect(G_OBJECT(this->window), "destroy", G_CALLBACK(on_destroy),
                   this);

  char title[8192];
  strcpy(title, this->repo_path);
  strcat(title, " - WeźMnie");
  gtk_window_set_title(GTK_WINDOW(this->window), title);

  if (this->repo == NULL) {
    const char *msg = "Nie udało się otworzyć pliku!";
    GtkWidget *dialog = gtk_message_dialog_new(
        GTK_WINDOW(this->window), GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "%s", msg);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    free(this);
    return false;
  }

  editor_semester_show(this->semester);
  editor_equipment_show(this->equipment);
  editor_reports_show(this->reports);

  gtk_widget_show_all(GTK_WIDGET(this->window));
  GObject *notebook = gtk_builder_get_object(this->ui, "notebook");
  gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), 0);
  return true;
}
