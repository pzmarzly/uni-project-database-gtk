#include "EditorSemester.h"
#include <stdbool.h>
#include <gtk/gtk.h>
#include "Editor.h"
#include "Repo.h"
#include "RepoData.h"
#include "RepoString.h"
#include "Utils.h"

struct EditorSemester {
    Repo *repo;
    GtkBuilder *ui;
};

EditorSemester* editor_semester_new(Repo *repo, GtkBuilder *ui) {
    EditorSemester *eq = malloc(sizeof(EditorSemester));
    eq->repo = repo;
    eq->ui = ui;
    return eq;
}

void editor_semester_show(EditorSemester *eq) {
    GObject *semester = gtk_builder_get_object(eq->ui, "semester");

    GtkWidget *week = gtk_button_new_with_label("Plan tygodnia");
    gtk_box_pack_start(GTK_BOX(semester), week, 0, 0, 0);
}
