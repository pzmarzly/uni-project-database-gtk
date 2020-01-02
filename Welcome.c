// Paweł Zmarzły 314569
// Projekt, grupa MPi śr 12-14

#include "Welcome.h"
#include <gtk/gtk.h>
#include <string.h>
#include "Recent.h"
#include "Editor.h"
#include "Utils.h"

struct Welcome {
    GtkBuilder *ui;
    bool quit_on_destroy;
    GObject *window;

    char *recent[MAX_RECENT];
    int recent_len;
    char *demo;
};

Welcome* welcome_new() {
    Welcome *this = malloc(sizeof(Welcome));

    char *glade = strcat(basedir(), "/Welcome.glade");
    this->ui = gtk_builder_new_from_file(glade);
    free(glade);

    this->quit_on_destroy = false;
    this->window = NULL;
    this->recent_len = 0;
    return this;
}

void welcome_set_quit_on_destroy(Welcome *this, bool quit_on_destroy) {
    this->quit_on_destroy = quit_on_destroy;
}

static void on_destroy(GtkWidget *sender, gpointer user_data) {
    (void)sender;
    Welcome *this = (Welcome *)user_data;
    if (this->quit_on_destroy)
        gtk_main_quit();
    free(this);
}

typedef struct {
    Welcome *this;
    char *path;
    bool overwrite;
    bool push_to_recent;
} LoadEditorRequest;

static LoadEditorRequest* prepare_load(
    Welcome *this,
    char *path,
    bool overwrite,
    bool push_to_recent
) {
    LoadEditorRequest *req = malloc(sizeof(LoadEditorRequest));
    req->this = this;
    req->path = g_strdup(path);
    req->overwrite = overwrite;
    req->push_to_recent = push_to_recent;
    return req;
}

static void load_editor(LoadEditorRequest *req) {
    Editor* editor = editor_new(req->path, req->overwrite);
    if (!editor_start(editor)) {
        return;
    }

    if (req->push_to_recent) {
        Recent *recent = recent_load();
        recent_push(recent, req->path);
    }

    welcome_set_quit_on_destroy(req->this, false);
    editor_set_quit_on_destroy(editor, true);
    gtk_widget_destroy(GTK_WIDGET(req->this->window));

    free(req->path);
    free(req);
}

static void on_btn_new(GtkWidget *sender, gpointer user_data) {
    (void)sender;
    Welcome *this = (Welcome *)user_data;

    GtkWidget *dialog = gtk_file_chooser_dialog_new(
        "Utwórz bazę",
        GTK_WINDOW(this->window),
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
        req = prepare_load(this, path, true, true);
        g_free(path);
    }
    gtk_widget_destroy(dialog);

    if (req != NULL)
        load_editor(req);
}

static void on_btn_open(GtkWidget *sender, gpointer user_data) {
    (void)sender;
    Welcome *this = (Welcome *)user_data;

    GtkWidget *dialog = gtk_file_chooser_dialog_new(
        "Wybierz bazę",
        GTK_WINDOW(this->window),
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
        req = prepare_load(this, path, false, true);
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

static bool on_demo_button_clicked(GtkWidget *sender, gpointer user_data) {
    LoadEditorRequest *req = (LoadEditorRequest *)user_data;

    char *temp = temp_file();
    if (temp == NULL) return false;
    if (!copy_file(req->path, temp)) return false;
    req->path = temp;

    return on_recent_label_clicked(sender, user_data);
}

static void make_recent_label(Welcome *this, char* path, GObject *box) {
    GtkWidget *recent_label = gtk_link_button_new_with_label(path, path);
    g_signal_connect(G_OBJECT(recent_label), "activate-link",
        G_CALLBACK(on_recent_label_clicked), prepare_load(this, path, false, true));
    gtk_box_pack_start(GTK_BOX(box), GTK_WIDGET(recent_label), 0, 0, 0);
}

bool welcome_start(Welcome *this) {
    this->window = gtk_builder_get_object(this->ui, "window");
    g_signal_connect(G_OBJECT(this->window), "destroy", G_CALLBACK(on_destroy), this);
    gtk_window_set_title(GTK_WINDOW(this->window), "WeźMnie");
    apply_css(GTK_WIDGET(this->window), "label, button { margin: 5px; }");

    GObject *btn_new = gtk_builder_get_object(this->ui, "new");
    GObject *btn_open = gtk_builder_get_object(this->ui, "open");
    g_signal_connect(G_OBJECT(btn_new), "clicked", G_CALLBACK(on_btn_new), this);
    g_signal_connect(G_OBJECT(btn_open), "clicked", G_CALLBACK(on_btn_open), this);

    Recent *recent = recent_load();
    GObject *recent_box = gtk_builder_get_object(this->ui, "recent");
    for (int i = 0; i < recent->items; i++) {
        make_recent_label(this, recent->paths[i], recent_box);
    }

    this->demo = strcat(basedir(), "/demo.db");
    GtkWidget *demo_btn = gtk_button_new_with_label("Utwórz bazę demonstracyjną...");
    g_signal_connect(G_OBJECT(demo_btn), "clicked",
        G_CALLBACK(on_demo_button_clicked), prepare_load(this, this->demo, false, false));
    gtk_box_pack_end(GTK_BOX(recent_box), GTK_WIDGET(demo_btn), 0, 0, 0);

    gtk_widget_show_all(GTK_WIDGET(this->window));
    return true;
}
