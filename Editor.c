// Paweł Zmarzły 314569
// Projekt, grupa MPi śr 12-14

#include "Editor.h"
#include <gtk/gtk.h>
#include "Repo.h"
#include "Utils.h"

struct Editor {
    GtkBuilder *ui;
    bool quit_on_destroy;
    GObject *window;
    Repo *repo;
};

Editor* editor_new(char *path, bool create) {
    Editor *re = malloc(sizeof(Editor));

    char *glade = strcat(basedir(), "/Editor.glade");
    re->ui = gtk_builder_new_from_file(glade);
    free(glade);

    re->quit_on_destroy = false;
    re->window = NULL;
    re->repo = repo_open(path, create);
    return re;
}

void editor_set_quit_on_destroy(Editor *re, bool quit_on_destroy) {
    re->quit_on_destroy = quit_on_destroy;
}

static void on_destroy(GtkWidget *sender, gpointer user_data) {
    (void)sender;
    Editor *re = (Editor *)user_data;
    if (re->quit_on_destroy)
        gtk_main_quit();
    repo_close(re->repo);
    free(re);
}

void editor_run(Editor *re) {
    if (re->repo == NULL) {
        return; // TODO: dialog
    }

    re->window = gtk_builder_get_object(re->ui, "window");
    g_signal_connect(G_OBJECT(re->window), "destroy", G_CALLBACK(on_destroy), re);

    gtk_widget_show_all(GTK_WIDGET(re->window));
}
