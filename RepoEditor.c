// Paweł Zmarzły 314569
// Projekt, grupa MPi śr 12-14

#include "RepoEditor.h"
#include <gtk/gtk.h>
#include "Utils.h"

struct RepoEditor {
    GtkBuilder *ui;
    bool quit_on_destroy;
    GObject *window;
    FILE *file;
    bool unsaved;
};

RepoEditor* repo_editor_new(char *path, bool create) {
    RepoEditor *re = malloc(sizeof(RepoEditor));

    char *glade = strcat(basedir(), "/RepoEditor.glade");
    re->ui = gtk_builder_new_from_file(glade);
    free(glade);

    re->quit_on_destroy = false;
    re->window = NULL;
    re->file = fopen(path, create ? "wb" : "r+b");
    re->unsaved = false;
    return re;
}

void repo_editor_set_quit_on_destroy(RepoEditor *re, bool quit_on_destroy) {
    re->quit_on_destroy = quit_on_destroy;
}

static void on_destroy(GtkWidget *sender, gpointer user_data) {
    (void)sender;
    RepoEditor *re = (RepoEditor *)user_data;
    if (re->quit_on_destroy)
        gtk_main_quit();
    fclose(re->file);
    free(re);
}

static bool on_delete(GtkWidget *sender, gpointer user_data) {
    (void)sender;
    RepoEditor *re = (RepoEditor *)user_data;
    if (re->unsaved) {
        // TODO: dialog
        return true;
    }
    return false;
}

void repo_editor_run(RepoEditor *re) {
    if (re->file == NULL) {
        return; // TODO: dialog
    }

    re->window = gtk_builder_get_object(re->ui, "window");
    g_signal_connect(G_OBJECT(re->window), "delete-event", G_CALLBACK(on_delete), re);
    g_signal_connect(G_OBJECT(re->window), "destroy", G_CALLBACK(on_destroy), re);

    gtk_widget_show_all(GTK_WIDGET(re->window));
}
