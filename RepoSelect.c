// Paweł Zmarzły 314569
// Projekt, grupa MPi śr 12-14

#include "RepoSelect.h"
#include <gtk/gtk.h>
#include "RepoRecent.h"
#include "Utils.h"

struct RepoSelect {
    GtkBuilder *ui;
    bool quit_on_destroy;
};

RepoSelect* repo_select_new() {
    RepoSelect *rs = malloc(sizeof(RepoSelect));
    char *xml = basedir();
    strcat(xml, "/RepoSelect.xml");
    rs->ui = gtk_builder_new_from_file(xml);
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

bool on_click(GtkWidget *sender, gpointer user_data) {
    printf("Clicked on %s\n", user_data);
    return true;
}

void make_recent(char* path, GObject *box) {
    GtkWidget *recent_label = gtk_link_button_new_with_label(path, path);
    g_signal_connect(G_OBJECT(recent_label), "activate-link", G_CALLBACK(on_click), path);
    gtk_box_pack_start(GTK_BOX(box), GTK_WIDGET(recent_label), 0, 0, 0);
}

void repo_select_run(RepoSelect *rs) {
    GObject *window = gtk_builder_get_object(rs->ui, "window");
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(on_destroy), rs);

    char *recent[5];
    int recent_len = repo_recent_load(recent, 5);
    GObject *recent_box = gtk_builder_get_object(rs->ui, "recent");
    for (int i = 0; i < recent_len; i++) {
        make_recent(recent[i], recent_box);
    }
    char *demo = basedir();
    strcat(demo, "/demo.db");
    make_recent(demo, recent_box);

    gtk_widget_show_all(GTK_WIDGET(window));
}
