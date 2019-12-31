// Paweł Zmarzły 314569
// Projekt, grupa MPi śr 12-14

#include "Editor.h"
#include <gtk/gtk.h>
#include "Repo.h"
#include "Utils.h"
#include "EditorSemester.h"
#include "EditorEquipment.h"
#include "EditorReports.h"

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

Editor* editor_new(char *path, bool overwrite) {
    Editor *this = malloc(sizeof(Editor));

    char *glade = strcat(basedir(), "/Editor.glade");
    this->ui = gtk_builder_new_from_file(glade);
    free(glade);

    this->quit_on_destroy = false;
    this->window = NULL;
    this->repo = repo_open(path, overwrite, 0);
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

bool editor_run(Editor *this) {
    this->window = gtk_builder_get_object(this->ui, "window");
    g_signal_connect(G_OBJECT(this->window), "destroy", G_CALLBACK(on_destroy), this);

    char title[8192];
    strcpy(title, this->repo_path);
    strcat(title, " - WeźMnie");
    gtk_window_set_title(GTK_WINDOW(this->window), title);

    if (this->repo == NULL) {
        const char *msg = "Nie udało się otworzyć pliku!";
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(this->window),
            GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "%s", msg);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        free(this);
        return false;
    }

    editor_semester_show(this->semester);
    editor_equipment_show(this->equipment);
    editor_reports_show(this->reports);

    gtk_widget_show_all(GTK_WIDGET(this->window));
    return true;
}
