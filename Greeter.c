// Paweł Zmarzły 314569
// Projekt, grupa MPi śr 12-14

#include "Greeter.h"
#include "About.h"
#include "Editor.h"
#include "RecentList.h"
#include "Utils.h"
#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>

struct Greeter {
  GtkBuilder *ui;
  bool quit_on_destroy;
  GObject *window;
};

Greeter *greeter_new() {
  Greeter *this = malloc(sizeof(Greeter));
  this->ui = get_builder("Greeter.glade");
  this->quit_on_destroy = false;
  this->window = NULL;
  return this;
}

void greeter_set_quit_on_destroy(Greeter *this, bool quit_on_destroy) {
  this->quit_on_destroy = quit_on_destroy;
}

static void on_destroy(GtkWidget *sender, gpointer user_data) {
  (void)sender;
  Greeter *this = (Greeter *)user_data;
  if (this->quit_on_destroy)
    gtk_main_quit();
  free(this);
}

typedef struct {
  Greeter *this;
  char *path;
  RepoType repo_type;
} LoadEditorRequest;

static LoadEditorRequest *prepare_load(Greeter *this, char *path,
                                       RepoType repo_type) {
  LoadEditorRequest *req = malloc(sizeof(LoadEditorRequest));
  req->this = this;
  req->path = g_strdup(path);
  req->repo_type = repo_type;
  return req;
}

static void load_editor(LoadEditorRequest *req) {
  Editor *editor = editor_new(req->path, req->repo_type);
  if (!editor_start(editor)) {
    RecentList *recent_list = recent_list_load();
    recent_list_del_all_equal(recent_list, req->path);
    recent_list_save(recent_list);
    return;
  }

  if (req->repo_type != RepoDemo) {
    RecentList *recent_list = recent_list_load();
    recent_list_push(recent_list, req->path);
    recent_list_save(recent_list);
  }

  greeter_set_quit_on_destroy(req->this, false);
  editor_set_quit_on_destroy(editor, true);
  gtk_widget_destroy(GTK_WIDGET(req->this->window));

  free(req->path);
  free(req);
}

static void on_btn_new(GtkWidget *sender, gpointer user_data) {
  (void)sender;
  Greeter *this = (Greeter *)user_data;

  GtkWidget *dialog = gtk_file_chooser_dialog_new(
      "Utwórz bazę", GTK_WINDOW(this->window), GTK_FILE_CHOOSER_ACTION_SAVE,
      "Anuluj", GTK_RESPONSE_CANCEL, "Utwórz", GTK_RESPONSE_ACCEPT, NULL);
  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), file_filter());
  gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog),
                                                 TRUE);
  gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), "baza.db");

  int res = gtk_dialog_run(GTK_DIALOG(dialog));
  LoadEditorRequest *req = NULL;
  if (res == GTK_RESPONSE_ACCEPT) {
    char *path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
    req = prepare_load(this, path, RepoNew);
    g_free(path);
  }
  gtk_widget_destroy(dialog);

  if (req != NULL)
    load_editor(req);
}

static void on_btn_open(GtkWidget *sender, gpointer user_data) {
  (void)sender;
  Greeter *this = (Greeter *)user_data;

  GtkWidget *dialog = gtk_file_chooser_dialog_new(
      "Wybierz bazę", GTK_WINDOW(this->window), GTK_FILE_CHOOSER_ACTION_SAVE,
      "Anuluj", GTK_RESPONSE_CANCEL, "Otwórz", GTK_RESPONSE_ACCEPT, NULL);
  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), file_filter());

  int res = gtk_dialog_run(GTK_DIALOG(dialog));
  LoadEditorRequest *req = NULL;
  if (res == GTK_RESPONSE_ACCEPT) {
    char *path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
    req = prepare_load(this, path, RepoOpen);
    g_free(path);
  }
  gtk_widget_destroy(dialog);

  if (req != NULL)
    load_editor(req);
}

static void on_btn_about(GtkWidget *sender, gpointer user_data) {
  (void)sender;
  (void)user_data;
  about();
}

static bool on_recent_list_label_clicked(GtkWidget *sender,
                                         gpointer user_data) {
  (void)sender;
  load_editor((LoadEditorRequest *)user_data);
  return true;
}

static bool on_demo_button_clicked(GtkWidget *sender, gpointer user_data) {
  LoadEditorRequest *req = (LoadEditorRequest *)user_data;

  char *temp = temp_file();
  if (temp == NULL)
    return false;
  if (!copy_file(req->path, temp))
    return false;
  req->path = temp;

  return on_recent_list_label_clicked(sender, user_data);
}

static void make_recent_list_label(Greeter *this, char *path, GObject *box) {
  GtkWidget *recent_list_label = gtk_link_button_new_with_label(path, path);
  g_signal_connect(G_OBJECT(recent_list_label), "activate-link",
                   G_CALLBACK(on_recent_list_label_clicked),
                   prepare_load(this, path, RepoOpen));
  gtk_box_pack_start(GTK_BOX(box), GTK_WIDGET(recent_list_label), 0, 0, 0);
}

bool greeter_start(Greeter *this) {
  this->window = gtk_builder_get_object(this->ui, "window");
  g_signal_connect(G_OBJECT(this->window), "destroy", G_CALLBACK(on_destroy),
                   this);
  gtk_window_set_title(GTK_WINDOW(this->window), "WeźMnie");
  apply_css(GTK_WIDGET(this->window), "label, .label { padding: 5px; }");

  GObject *btn_new = gtk_builder_get_object(this->ui, "new");
  GObject *btn_open = gtk_builder_get_object(this->ui, "open");
  GObject *btn_about = gtk_builder_get_object(this->ui, "about");
  g_signal_connect(G_OBJECT(btn_new), "clicked", G_CALLBACK(on_btn_new), this);
  g_signal_connect(G_OBJECT(btn_open), "clicked", G_CALLBACK(on_btn_open),
                   this);
  g_signal_connect(G_OBJECT(btn_about), "clicked", G_CALLBACK(on_btn_about),
                   NULL);

  RecentList *recent_list = recent_list_load();
  GObject *recent_list_box = gtk_builder_get_object(this->ui, "recent_list");
  for (int i = 0; i < recent_list->items; i++) {
    make_recent_list_label(this, recent_list->paths[i], recent_list_box);
  }

  char *demo_path = program_dir(8);
  strcat(demo_path, "/demo.db");
  GtkWidget *demo_btn =
      gtk_button_new_with_label("Utwórz bazę demonstracyjną...");
  g_signal_connect(G_OBJECT(demo_btn), "clicked",
                   G_CALLBACK(on_demo_button_clicked),
                   prepare_load(this, demo_path, RepoDemo));
  gtk_box_pack_end(GTK_BOX(recent_list_box), GTK_WIDGET(demo_btn), 0, 0, 0);
  free(demo_path);

  gtk_widget_show_all(GTK_WIDGET(this->window));
  return true;
}
