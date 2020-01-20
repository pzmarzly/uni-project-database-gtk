#include "EditorSemester.h"
#include "EditorDialogs.h"
#include "RecentList.h"
#include "Repo.h"
#include "RepoData.h"
#include "RepoString.h"
#include "Utils.h"

struct EditorSemester {
  Repo *repo;
  GtkBuilder *ui;
  GtkWindow *parent;
  Editor *editor;
};

EditorSemester *editor_semester_new(Repo *repo, GtkBuilder *ui,
                                    Editor *editor) {
  EditorSemester *this = malloc(sizeof(EditorSemester));
  this->repo = repo;
  this->ui = ui;
  this->parent = GTK_WINDOW(gtk_builder_get_object(ui, "window"));
  this->editor = editor;
  return this;
}

static void do_save_as(EditorSemester *this, bool start_fresh) {
  Timestamp new_start, new_end;
  if (start_fresh)
    if (!ask_for_semester_dates(&new_start, &new_end))
      return;

  GtkWidget *dialog = gtk_file_chooser_dialog_new(
      "Zapisz bazę", GTK_WINDOW(this->parent), GTK_FILE_CHOOSER_ACTION_SAVE,
      "Anuluj", GTK_RESPONSE_CANCEL, "Zapisz", GTK_RESPONSE_ACCEPT, NULL);
  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), file_filter());
  gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog),
                                                 TRUE);
  gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), "kopia.db");

  int res = gtk_dialog_run(GTK_DIALOG(dialog));
  if (res == GTK_RESPONSE_ACCEPT) {
    char *path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
    char *old_path = repo_get_path(this->repo);
    if (strcmp(path, old_path) == 0) {
      gtk_widget_destroy(dialog);
      free(old_path);
      free(path);
      return;
    }

    if (start_fresh) {
      Repo *new_repo = repo_open(path, RepoNew, new_start, new_end);
      ID max = repo_len(this->repo, TableEquipment);
      for (ID i = 0; i < max; i++) {
        // Copy just the equipment (and its description).
        Equipment e;
        repo_get(this->repo, TableEquipment, i, &e);
        char *s;
        repo_string_get(this->repo, e.description, &s);
        // We don't want unnecessary gaps in TableStringMetadata.
        e.description = repo_string_len(new_repo);
        repo_string_set(new_repo, e.description, &s);
        repo_set(new_repo, TableEquipment, i, &e);
        free(s);
      }
    } else {
      char *cur_path = repo_get_path(this->repo);
      if (!copy_file(cur_path, path))
        error("Nie udało się utworzyć kopii");
      free(cur_path);
    }

    RecentList *recent_list = recent_list_load();
    recent_list_push(recent_list, path);

    gtk_widget_destroy(dialog);

    Editor *editor = editor_new(path, RepoOpen);
    free(old_path);
    free(path);
    editor_start(editor);
    editor_set_quit_on_destroy(this->editor, false);
    editor_set_quit_on_destroy(editor, true);
    gtk_widget_destroy(GTK_WIDGET(this->parent));
  } else {
    gtk_widget_destroy(dialog);
  }
}

static void on_export(GtkWidget *sender, gpointer user_data) {
  (void)sender;
  EditorSemester *this = (EditorSemester *)user_data;

  PreparedEditDialog d = dialog_edit("Ustawienia kopii");
  GtkBuilder *ui = d.ui;
  GtkDialog *dialog = d.dialog;
  GtkGrid *grid = GTK_GRID(gtk_builder_get_object(ui, "grid"));

  GtkWidget *clean_btn = gtk_check_button_new_with_label(
      "Skopiuj tylko sprzęt (utwórz nowy semestr)");
  gtk_grid_attach(grid, clean_btn, 0, 0, 1, 1);

  gtk_widget_show_all(GTK_WIDGET(dialog));
  int res = gtk_dialog_run(dialog);
  bool start_fresh = false;
  bool ok = res == GTK_RESPONSE_OK;
  if (ok)
    start_fresh = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(clean_btn));
  gtk_widget_destroy(GTK_WIDGET(dialog));
  if (ok)
    do_save_as(this, start_fresh);
}

void editor_semester_show(EditorSemester *this) {
  char *text;

  GObject *start = gtk_builder_get_object(this->ui, "semester-start");
  text = timestamp_day_str(repo_get_semester_start(this->repo));
  gtk_label_set_text(GTK_LABEL(start), text);
  free(text);

  GObject *end = gtk_builder_get_object(this->ui, "semester-end");
  text = timestamp_day_str(repo_get_semester_end(this->repo));
  gtk_label_set_text(GTK_LABEL(end), text);
  free(text);

  GObject *export_btn = gtk_builder_get_object(this->ui, "semester-export");
  g_signal_connect(G_OBJECT(export_btn), "clicked", G_CALLBACK(on_export),
                   this);
}
