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
    GObject *start_btn = gtk_builder_get_object(eq->ui, "semester-start");
    GObject *active_btn = gtk_builder_get_object(eq->ui, "semester-active");
    GObject *save_as_btn = gtk_builder_get_object(eq->ui, "semester-save-as");

    gtk_button_set_label(GTK_BUTTON(start_btn), "");
    gtk_button_set_label(GTK_BUTTON(active_btn), "");
}
