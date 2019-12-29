// Paweł Zmarzły 314569
// Projekt, grupa MPi śr 12-14

#include "RepoSelect.h"
#include <gtk/gtk.h>
#include "RepoRecent.h"

struct RepoSelect {
    GtkBuilder *ui;
    bool quit_on_destroy;
};

RepoSelect* repo_select_new() {
    RepoSelect *rs = malloc(sizeof(RepoSelect));
    rs->ui = gtk_builder_new_from_file("RepoSelect.xml");
    rs->quit_on_destroy = false;
    return rs;
}

void repo_select_set_quit_on_destroy(RepoSelect *rs, bool quit_on_destroy) {
    rs->quit_on_destroy = quit_on_destroy;
}

void on_destroy(GtkWidget *sender, gpointer user_data) {
    (void)sender;
    RepoSelect *rs = (RepoSelect *)user_data;
    if (rs->quit_on_destroy)
        gtk_main_quit();
    free(rs);
}

void on_click(GtkWidget *sender, gpointer user_data) {
    printf("Click\n");
}

void repo_select_run(RepoSelect *rs) {
    GObject *window = gtk_builder_get_object(rs->ui, "window");
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(on_destroy), rs);

    char *recent[5];
    int recent_len = repo_recent_load(recent, 5);
    GObject *recent_box = gtk_builder_get_object(rs->ui, "recent");

    GtkWidget *event_box = gtk_event_box_new();
    GtkWidget *recent_label = gtk_label_new("Test. <a href=\"http://github.com/\" target=\"_blank\">Test</a>");
    g_signal_connect(G_OBJECT(event_box), "button-press-event", G_CALLBACK(on_click), NULL);
    gtk_label_set_xalign(GTK_LABEL(recent_label), 0.1);
    gtk_container_add(GTK_CONTAINER(event_box), GTK_WIDGET(recent_label));
    gtk_box_pack_start(GTK_BOX(recent_box), GTK_WIDGET(event_box), 0, 0, 0);

    gtk_widget_show_all(GTK_WIDGET(window));
}
