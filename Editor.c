// Paweł Zmarzły 314569
// Projekt, grupa MPi śr 12-14

#include "Editor.h"
#include "EditorDialogs.h"
#include "EditorEquipment.h"
#include "EditorOneTimeReservation.h"
#include "EditorPeriodicReservation.h"
#include "EditorReports.h"
#include "EditorSemester.h"
#include "RepoData.h"
#include "Utils.h"

struct Editor {
  GtkBuilder *ui;
  bool quit_on_destroy;
  GObject *window;
  Repo *repo;
  char *repo_path;

  EditorSemester *semester;
  EditorEquipment *equipment;
  EditorPeriodicReservation *periodic_reservation;
  EditorOneTimeReservation *one_time_reservation;
  EditorReports *reports;
};

Editor *editor_new(char *path, RepoType repo_type) {
  Editor *this = malloc(sizeof(Editor));
  this->ui = get_builder("Editor.glade");
  this->quit_on_destroy = false;
  this->window = NULL;
  Timestamp start = 0, end = 10000;
  if (repo_type == RepoNew)
    // If users cancels creation of a new repository,
    // we just exit.
    if (!ask_for_semester_dates(&start, &end))
      error("Nie wybrano daty!");
  this->repo = repo_open(path, repo_type, start, end);
  this->repo_path = g_strdup(path);
  this->semester = editor_semester_new(this->repo, this->ui, this);
  this->equipment = editor_equipment_new(this->repo, this->ui);
  this->periodic_reservation =
      editor_periodic_reservation_new(this->repo, this->ui);
  this->one_time_reservation =
      editor_one_time_reservation_new(this->repo, this->ui);
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
  editor_periodic_reservation_show(this->periodic_reservation);
  editor_one_time_reservation_show(this->one_time_reservation);
  editor_reports_show(this->reports);

  gtk_widget_show_all(GTK_WIDGET(this->window));
  GObject *notebook = gtk_builder_get_object(this->ui, "notebook");
  gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), 0);
  return true;
}
