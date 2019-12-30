// Paweł Zmarzły 314569
// Projekt, grupa MPi śr 12-14

#include "Welcome.h"
#include <gtk/gtk.h>
#include <string.h>
#include "Recent.h"
#include "Editor.h"
#include "Utils.h"

#define MAX_RECENT 5

struct Welcome {
    GtkBuilder *ui;
    bool quit_on_destroy;
    GObject *window;

    char *recent[MAX_RECENT];
    int recent_len;
    char *demo;
};

Welcome* welcome_new() {
    Welcome *rs = malloc(sizeof(Welcome));

    char *glade = strcat(basedir(), "/Welcome.glade");
    rs->ui = gtk_builder_new_from_file(glade);
    free(glade);

    rs->quit_on_destroy = false;
    rs->window = NULL;
    rs->recent_len = 0;
    return rs;
}

void welcome_set_quit_on_destroy(Welcome *rs, bool quit_on_destroy) {
    rs->quit_on_destroy = quit_on_destroy;
}

static void on_destroy(GtkWidget *sender, gpointer user_data) {
    (void)sender;
    Welcome *rs = (Welcome *)user_data;
    if (rs->quit_on_destroy)
        gtk_main_quit();
    free(rs);
}

typedef struct {
    Welcome *rs;
    char *path;
    bool create;
} LoadEditorRequest;

static LoadEditorRequest* prepare_request(Welcome *rs, char *path, bool create) {
    LoadEditorRequest *req = malloc(sizeof(LoadEditorRequest));
    req->rs = rs;
    req->path = g_strdup(path);
    req->create = create;
    return req;
}

static void load_editor(LoadEditorRequest *req) {
    if (strcmp(req->path, req->rs->demo) != 0)
        recent_push(req->rs->recent, req->rs->recent_len, MAX_RECENT, req->path);

    welcome_set_quit_on_destroy(req->rs, false);
    gtk_widget_destroy(GTK_WIDGET(req->rs->window));

    Editor* re = editor_new(req->path, req->create);
    editor_set_quit_on_destroy(re, true);
    editor_run(re);

    free(req->path);
    free(req);
}

static void on_btn_new(GtkWidget *sender, gpointer user_data) {
    (void)sender;
    Welcome *rs = (Welcome *)user_data;

    GtkWidget *dialog = gtk_file_chooser_dialog_new(
        "Utwórz bazę",
        GTK_WINDOW(rs->window),
        GTK_FILE_CHOOSER_ACTION_SAVE,
        "Anuluj",
        GTK_RESPONSE_CANCEL,
        "Utwórz",
        GTK_RESPONSE_ACCEPT,
        NULL
    );
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), file_filter());
    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);
    gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), "baza.db");

    int res = gtk_dialog_run(GTK_DIALOG(dialog));
    LoadEditorRequest *req = NULL;
    if (res == GTK_RESPONSE_ACCEPT) {
        char *path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        req = prepare_request(rs, path, true);
        g_free(path);
    }
    gtk_widget_destroy(dialog);

    if (req != NULL)
        load_editor(req);
}

static void on_btn_open(GtkWidget *sender, gpointer user_data) {
    (void)sender;
    Welcome *rs = (Welcome *)user_data;

    GtkWidget *dialog = gtk_file_chooser_dialog_new(
        "Wybierz bazę",
        GTK_WINDOW(rs->window),
        GTK_FILE_CHOOSER_ACTION_SAVE,
        "Anuluj",
        GTK_RESPONSE_CANCEL,
        "Otwórz",
        GTK_RESPONSE_ACCEPT,
        NULL
    );
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), file_filter());

    int res = gtk_dialog_run(GTK_DIALOG(dialog));
    LoadEditorRequest *req = NULL;
    if (res == GTK_RESPONSE_ACCEPT) {
        char *path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        req = prepare_request(rs, path, false);
        g_free(path);
    }
    gtk_widget_destroy(dialog);

    if (req != NULL)
        load_editor(req);
}

static bool on_recent_label_clicked(GtkWidget *sender, gpointer user_data) {
    (void)sender;
    load_editor((LoadEditorRequest *)user_data);
    return true;
}

static void make_recent_label(Welcome *rs, char* path, GObject *box) {
    GtkWidget *recent_label = gtk_link_button_new_with_label(path, path);
    g_signal_connect(G_OBJECT(recent_label), "activate-link",
        G_CALLBACK(on_recent_label_clicked), prepare_request(rs, path, false));
    gtk_box_pack_start(GTK_BOX(box), GTK_WIDGET(recent_label), 0, 0, 0);
}

void welcome_run(Welcome *rs) {
    rs->window = gtk_builder_get_object(rs->ui, "window");
    g_signal_connect(G_OBJECT(rs->window), "destroy", G_CALLBACK(on_destroy), rs);
    apply_css(GTK_WIDGET(rs->window), "label, button { margin: 5px; }");

    GObject *btn_new = gtk_builder_get_object(rs->ui, "new");
    GObject *btn_open = gtk_builder_get_object(rs->ui, "open");
    g_signal_connect(G_OBJECT(btn_new), "clicked", G_CALLBACK(on_btn_new), rs);
    g_signal_connect(G_OBJECT(btn_open), "clicked", G_CALLBACK(on_btn_open), rs);

    rs->recent_len = recent_load(rs->recent, MAX_RECENT);
    GObject *recent_box = gtk_builder_get_object(rs->ui, "recent");
    for (int i = 0; i < rs->recent_len; i++) {
        make_recent_label(rs, rs->recent[i], recent_box);
    }

    rs->demo = strcat(basedir(), "/demo.db");
    make_recent_label(rs, rs->demo, recent_box);

    gtk_widget_show_all(GTK_WIDGET(rs->window));
}
