// Paweł Zmarzły 314569
// Projekt, grupa MPi śr 12-14

#include "RepoSelect.h"
#include <gtk/gtk.h>
#include "RepoRecents.h"

struct RepoSelect {
    GtkBuilder *ui;
    bool quit_on_destroy;
};

RepoSelect repo_select_new() {
    App ret = {
        .ui = gtk_builder_new_from_file("RepoSelect.xml"),
        .quit_on_close = false,
    };
    return ret;
}

void repo_select_set_quit_on_destroy(RepoSelect *rs, bool quit_on_destroy) {
    rs->quit_on_destroy = quit_on_destroy;
}

void on_destroy(GtkObject *sender, gpointer user_data) {
    RepoSelect *rs = (RepoSelect *)user_data;
    if (rs->quit_on_destroy)
        gtk_main_quit();
}

void repo_select_run(RepoSelect *rs) {
    GtkWindow *window = gtk_builder_get_object(rs->ui, "window");
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(on_destroy), rs);

    char *recent[5];
    int recent_len = repo_recent_load(recent, 5);
    GtkLabel *recent = gtk_builder_get_object(rs->ui, "recent");
    gtk_label_set_text(recent, "Test. <a href=\"http://github.com/\" target=\"_blank\">Test</a>");

    gtk_widget_show_all(window);
}
