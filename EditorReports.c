#include "EditorReports.h"
#include "Editor.h"
#include "EditorDialogs.h"
#include "EditorReportsDialogs.h"
#include "Repo.h"
#include "RepoData.h"
#include "RepoString.h"
#include "Utils.h"
#include <stdbool.h>
#include <stdlib.h>

struct EditorReports {
  Editor *editor;
  Repo *repo;
  GtkBuilder *ui;
};

EditorReports *editor_reports_new(Editor *editor, Repo *repo, GtkBuilder *ui) {
  EditorReports *this = (EditorReports *)smalloc(sizeof(EditorReports));
  this->editor = editor;
  this->repo = repo;
  this->ui = ui;
  return this;
}

static void week_clicked(GtkWidget *sender, gpointer user_data) {
  (void)sender;
  EditorReports *this = (EditorReports *)user_data;
  Timestamp week_start;
  if (!ask_for_week_start(&week_start))
    return;
  dialog_week_summary(this->repo, week_start);
}

static void available_clicked(GtkWidget *sender, gpointer user_data) {
  (void)sender;
  EditorReports *this = (EditorReports *)user_data;
  Timestamp moment;
  if (!ask_for_moment(&moment))
    return;
  dialog_available_summary(this->repo, moment);
}

static void availability_clicked(GtkWidget *sender, gpointer user_data) {
  (void)sender;
  EditorReports *this = (EditorReports *)user_data;
  dialog_availability_ranking(this->repo);
}

void editor_reports_prepare(EditorReports *this) {
  GObject *reports = gtk_builder_get_object(this->ui, "reports");
  remove_all_gtk_children(GTK_CONTAINER(reports));

  GtkWidget *week = gtk_button_new_with_label("Plan tygodnia");
  gtk_box_pack_start(GTK_BOX(reports), week, 0, 0, 0);
  g_signal_connect(G_OBJECT(week), "clicked", G_CALLBACK(week_clicked), this);

  GtkWidget *available = gtk_button_new_with_label("Lista wolnego sprzętu");
  gtk_box_pack_start(GTK_BOX(reports), available, 0, 0, 0);
  g_signal_connect(G_OBJECT(available), "clicked",
                   G_CALLBACK(available_clicked), this);

  GtkWidget *availability =
      gtk_button_new_with_label("Ranking sprzętu wg dostępności");
  gtk_box_pack_start(GTK_BOX(reports), availability, 0, 0, 0);
  g_signal_connect(G_OBJECT(availability), "clicked",
                   G_CALLBACK(availability_clicked), this);
}

void editor_reports_refresh(EditorReports *this) { (void)this; }
