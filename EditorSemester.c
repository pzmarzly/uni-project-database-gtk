#include "EditorSemester.h"
#include <stdbool.h>
#include <gtk/gtk.h>
#include "Repo.h"
#include "Recent.h"
#include "Utils.h"

struct EditorSemester {
    Repo *repo;
    GtkBuilder *ui;
    GtkWindow *parent;
};

EditorSemester* editor_semester_new(Repo *repo, GtkBuilder *ui) {
    EditorSemester *this = malloc(sizeof(EditorSemester));
    this->repo = repo;
    this->ui = ui;
    this->parent = GTK_WINDOW(gtk_builder_get_object(ui, "window"));
    return this;
}

static void on_save_as(GtkWidget *sender, gpointer user_data) {
    (void)sender;
    EditorSemester *this = (EditorSemester *)user_data;

    GtkWidget *dialog = gtk_file_chooser_dialog_new(
        "Zapisz bazę",
        GTK_WINDOW(this->parent),
        GTK_FILE_CHOOSER_ACTION_SAVE,
        "Anuluj",
        GTK_RESPONSE_CANCEL,
        "Zapisz",
        GTK_RESPONSE_ACCEPT,
        NULL
    );
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), file_filter());
    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);
    gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), "kopia.db");

    int res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT) {
        char *path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        char *old_path = repo_get_path(this->repo);
        if (strcmp(path, old_path) == 0) {
            free(old_path);
            free(path);
            return;
        }
        Repo *old_repo = repo_save_as(this->repo, path);
        repo_close(old_repo);

        char title[8192];
        strcpy(title, path);
        strcat(title, " - WeźMnie");
        gtk_window_set_title(this->parent, title);

        Recent *recent = recent_load();
        recent_push(recent, path);
        free(old_path);
        free(path);
    }
    gtk_widget_destroy(dialog);
}

static void update_active_btn(GtkButton *button, bool active) {
    gtk_button_set_label(button, active ? "Zatrzymaj semestr" : "Rozpocznij semestr");
}

void on_active(GtkWidget *sender, gpointer user_data) {
    EditorSemester *this = (EditorSemester *)user_data;
    bool active = repo_get_semester_active(this->repo);
    // TODO: dialog
    active = !active;
    repo_set_semester_active(this->repo, active);
    update_active_btn(GTK_BUTTON(sender), active);
}

void editor_semester_show(EditorSemester *this) {
    GObject *start_btn = gtk_builder_get_object(this->ui, "semester-start");

    GObject *active_btn = gtk_builder_get_object(this->ui, "semester-active");
    bool active = repo_get_semester_active(this->repo);
    update_active_btn(GTK_BUTTON(active_btn), active);
    g_signal_connect(G_OBJECT(active_btn), "clicked", G_CALLBACK(on_active), this);

    GObject *save_as_btn = gtk_builder_get_object(this->ui, "semester-save-as");
    g_signal_connect(G_OBJECT(save_as_btn), "clicked", G_CALLBACK(on_save_as), this);

    gtk_button_set_label(GTK_BUTTON(start_btn), "");
}
