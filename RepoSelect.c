// Paweł Zmarzły 314569
// Projekt, grupa MPi śr 12-14

#include "RepoSelect.h"
#define _POSIX_C_SOURCE 200809L
#include <gtk/gtk.h>
#include <string.h>
#include "RepoRecent.h"
#include "RepoEditor.h"
#include "Utils.h"

struct RepoSelect {
    GtkBuilder *ui;
    bool quit_on_destroy;
    GObject *window;
};

RepoSelect* repo_select_new() {
    RepoSelect *rs = malloc(sizeof(RepoSelect));

    char *glade = basedir();
    strcat(glade, "/RepoSelect.glade");
    rs->ui = gtk_builder_new_from_file(glade);
    free(glade);

    rs->quit_on_destroy = false;
    rs->window = NULL;

    return rs;
}

void repo_select_set_quit_on_destroy(RepoSelect *rs, bool quit_on_destroy) {
    rs->quit_on_destroy = quit_on_destroy;
}

static void on_destroy(GtkWidget *sender, gpointer user_data) {
    (void)sender;
    RepoSelect *rs = (RepoSelect *)user_data;
    if (rs->quit_on_destroy)
        gtk_main_quit();
    free(rs);
}

typedef struct {
    RepoSelect *rs;
    char *path;
} LoadRepoEditorRequest;

static void load_repo_editor(LoadRepoEditorRequest *req) {
    repo_select_set_quit_on_destroy(req->rs, false);
    gtk_widget_destroy(GTK_WIDGET(req->rs->window));

    RepoEditor* re = repo_editor_new(req->path);
    repo_editor_set_quit_on_destroy(re, true);
    repo_editor_run(re);

    free(req->path);
    free(req);
}

static bool load_recent(GtkWidget *sender, gpointer user_data) {
    (void)sender;
    load_repo_editor((LoadRepoEditorRequest *)user_data);
    return true;
}

static void make_recent(RepoSelect *rs, char* path, GObject *box) {
    LoadRepoEditorRequest *req = malloc(sizeof(LoadRepoEditorRequest));
    req->rs = rs;
    req->path = strdup(path);

    GtkWidget *recent_label = gtk_link_button_new_with_label(path, path);
    g_signal_connect(G_OBJECT(recent_label), "activate-link", G_CALLBACK(load_recent), req);
    gtk_box_pack_start(GTK_BOX(box), GTK_WIDGET(recent_label), 0, 0, 0);
}

void repo_select_run(RepoSelect *rs) {
    rs->window = gtk_builder_get_object(rs->ui, "window");
    g_signal_connect(G_OBJECT(rs->window), "destroy", G_CALLBACK(on_destroy), rs);

    char *recent[5];
    int recent_len = repo_recent_load(recent, 5);
    GObject *recent_box = gtk_builder_get_object(rs->ui, "recent");
    for (int i = 0; i < recent_len; i++) {
        make_recent(rs, recent[i], recent_box);
    }

    char *demo = basedir();
    strcat(demo, "/demo.db");
    make_recent(rs, demo, recent_box);
    free(demo);

    gtk_widget_show_all(GTK_WIDGET(rs->window));
}
