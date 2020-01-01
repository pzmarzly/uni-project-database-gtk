#include "EditorReports.h"
#include <stdbool.h>
#include <gtk/gtk.h>
#include "Editor.h"
#include "Repo.h"
#include "RepoData.h"
#include "RepoString.h"
#include "Utils.h"

struct EditorReports {
    Repo *repo;
    GtkBuilder *ui;
};

EditorReports* editor_reports_new(Repo *repo, GtkBuilder *ui) {
    EditorReports *this = malloc(sizeof(EditorReports));
    this->repo = repo;
    this->ui = ui;
    return this;
}

void editor_reports_show(EditorReports *this) {
    GObject *reports = gtk_builder_get_object(this->ui, "reports");

    GtkWidget *week = gtk_button_new_with_label("Plan tygodnia");
    gtk_box_pack_start(GTK_BOX(reports), week, 0, 0, 0);

    GtkWidget *available = gtk_button_new_with_label("Lista wolnego sprzętu");
    gtk_box_pack_start(GTK_BOX(reports), available, 0, 0, 0);

    GtkWidget *ranking = gtk_button_new_with_label("Ranking sprzętu wg dostępności");
    gtk_box_pack_start(GTK_BOX(reports), ranking, 0, 0, 0);
}
