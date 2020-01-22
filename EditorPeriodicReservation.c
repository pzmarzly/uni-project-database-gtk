#include "EditorPeriodicReservation.h"
#include "Datepicker.h"
#include "Editor.h"
#include "EditorDialogs.h"
#include "Repo.h"
#include "RepoData.h"
#include "RepoString.h"
#include "Utils.h"
#include <gtk/gtk.h>
#include <stdbool.h>

struct EditorPeriodicReservation {
  Repo *repo;
  GtkBuilder *ui;
};

EditorPeriodicReservation *editor_periodic_reservation_new(Repo *repo,
                                                           GtkBuilder *ui) {
  EditorPeriodicReservation *this = malloc(sizeof(EditorPeriodicReservation));
  this->repo = repo;
  this->ui = ui;
  return this;
}

void periodic_reservation_refresh(EditorPeriodicReservation *this) {
  GObject *periodic_reservation =
      gtk_builder_get_object(this->ui, "periodic-reservation");
  remove_all_gtk_children(GTK_CONTAINER(periodic_reservation));
  editor_periodic_reservation_show(this);
  gtk_widget_show_all(GTK_WIDGET(periodic_reservation));
}

typedef struct {
  EditorPeriodicReservation *this;
  bool empty;
  ID id;
} EditRequest;

static EditRequest *prepare_edit(EditorPeriodicReservation *this, bool empty,
                                 ID id) {
  EditRequest *req = malloc(sizeof(EditRequest));
  req->this = this;
  req->empty = empty;
  req->id = id;
  return req;
}

static void on_edit(GtkWidget *sender, gpointer user_data) {
  (void)sender;
  EditRequest *req = (EditRequest *)user_data;

  PeriodicReservation r = {
      .item = INVALID_ID,
      .day = Monday,
      .start = 0,
      .end = 0,
      .active_since = 0,
      .active_until = 0,
      .description = 0,
  };
  char *desc;
  if (!req->empty) {
    repo_get(req->this->repo, TablePeriodicReservation, req->id, &r);
    repo_string_get(req->this->repo, r.description, &desc);
  } else {
    req->id = repo_len(req->this->repo, TablePeriodicReservation);
    desc = g_strdup("");
    r.description = repo_string_len(req->this->repo);
    r.active_since = repo_get_semester_start(req->this->repo);
    if (timestamp_now() > r.active_since)
      r.active_since = timestamp_now();
    r.active_until = repo_get_semester_end(req->this->repo);
  }

  PreparedEditDialog d = editor_edit_dialog_prepare(TablePeriodicReservation);
  GtkBuilder *ui = d.ui;
  GtkDialog *dialog = d.dialog;
  GtkGrid *grid = GTK_GRID(gtk_builder_get_object(ui, "grid"));

  GtkWidget *day_label = GTK_WIDGET(gtk_label_new("Dzień:"));
  gtk_grid_attach(grid, day_label, 0, 0, 1, 1);
  GtkWidget *start_label =
      GTK_WIDGET(gtk_label_new("Godzina rozpoczęcia:"));
  gtk_grid_attach(grid, start_label, 0, 1, 1, 1);
  GtkWidget *end_label =
      GTK_WIDGET(gtk_label_new("Godzina zakończenia:"));
  gtk_grid_attach(grid, end_label, 0, 2, 1, 1);
  GtkWidget *active_since_label = GTK_WIDGET(gtk_label_new("Aktywne od:"));
  gtk_grid_attach(grid, active_since_label, 0, 3, 1, 1);
  GtkWidget *active_until_label = GTK_WIDGET(gtk_label_new("Aktywne do:"));
  gtk_grid_attach(grid, active_until_label, 0, 4, 1, 1);
  GtkWidget *description_label = GTK_WIDGET(gtk_label_new("Opis:"));
  gtk_grid_attach(grid, description_label, 0, 5, 1, 1);

  GtkComboBox *day_combo_box = GTK_COMBO_BOX(gtk_combo_box_text_new());
  for (int i = 0; i < 7; i++)
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(day_combo_box), NULL,
                              day_str(i));
  gtk_combo_box_set_active(GTK_COMBO_BOX(day_combo_box), r.day);
  gtk_grid_attach(grid, GTK_WIDGET(day_combo_box), 1, 0, 1, 1);

  GtkEntry *start_entry = GTK_ENTRY(gtk_entry_new());
  gtk_entry_set_text(start_entry, hour_str(r.start));
  gtk_entry_set_max_length(start_entry, 3);
  gtk_grid_attach(grid, GTK_WIDGET(start_entry), 1, 1, 1, 1);

  GtkEntry *end_entry = GTK_ENTRY(gtk_entry_new());
  gtk_entry_set_text(end_entry, hour_str(r.end));
  gtk_entry_set_max_length(end_entry, 3);
  gtk_grid_attach(grid, GTK_WIDGET(end_entry), 1, 2, 1, 1);

  GtkButton *active_since_button = GTK_BUTTON(gtk_button_new_with_label(""));
  Datepicker *active_since =
      datepicker_new(active_since_button, r.active_since, NULL, NULL);
  gtk_grid_attach(grid, GTK_WIDGET(active_since_button), 1, 3, 1, 1);

  GtkButton *active_until_button = GTK_BUTTON(gtk_button_new_with_label(""));
  Datepicker *active_until =
      datepicker_new(active_until_button, r.active_until, NULL, NULL);
  gtk_grid_attach(grid, GTK_WIDGET(active_until_button), 1, 4, 1, 1);

  GtkTextView *description_text_view = GTK_TEXT_VIEW(gtk_text_view_new());
  GtkTextBuffer *buf =
      gtk_text_view_get_buffer(GTK_TEXT_VIEW(description_text_view));
  gtk_text_buffer_insert_at_cursor(buf, desc, -1);
  free(desc);
  gtk_grid_attach(grid, GTK_WIDGET(description_text_view), 1, 5, 1, 1);
  gtk_widget_set_hexpand(GTK_WIDGET(description_text_view), true);
  gtk_widget_set_vexpand(GTK_WIDGET(description_text_view), true);

  gtk_widget_show_all(GTK_WIDGET(dialog));

  while (true) {
    int result = gtk_dialog_run(dialog);
    if (result != GTK_RESPONSE_OK)
      break;

    r.day = gtk_combo_box_get_active(day_combo_box);

    const char *start_str = gtk_entry_get_text(start_entry);
    r.start = hour_parse(start_str);
    const char *end_str = gtk_entry_get_text(end_entry);
    r.end = hour_parse(end_str);
    if (r.start == HOUR_INVALID || r.end == HOUR_INVALID) {
      validation_error("Niepoprawny format godziny!");
      continue;
    }

    r.active_since = datepicker_read(active_since);
    r.active_until = datepicker_read(active_until);
    if (r.active_since >= r.active_until) {
      validation_error("Data startu nie może być wcześniejsza niż data końca!");
      continue;
    }

    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buf, &start, &end);
    desc = gtk_text_buffer_get_text(buf, &start, &end, FALSE);

    if (!ask_for_item_periodic(&r, req->id, req->this->repo))
      continue;

    repo_string_set(req->this->repo, r.description, &desc);
    repo_set(req->this->repo, TablePeriodicReservation, req->id, &r);
    periodic_reservation_refresh(req->this);
    break;
  }
  gtk_widget_destroy(GTK_WIDGET(dialog));
}

typedef struct {
  EditorPeriodicReservation *this;
  ID id;
} DelRequest;

static DelRequest *prepare_del(EditorPeriodicReservation *this, ID id) {
  DelRequest *req = malloc(sizeof(DelRequest));
  req->this = this;
  req->id = id;
  return req;
}

static void on_del(GtkWidget *sender, gpointer user_data) {
  (void)sender;
  DelRequest *req = (DelRequest *)user_data;

  PeriodicReservation r;
  repo_get(req->this->repo, TablePeriodicReservation, req->id, &r);
  char *name = describe_periodic_reservation(req->this->repo, &r);
  if (editor_removal_dialog(TablePeriodicReservation, name)) {
    repo_string_del(req->this->repo, r.description);
    repo_del(req->this->repo, TablePeriodicReservation, req->id);
    periodic_reservation_refresh(req->this);
  }
  free(name);
}

void editor_periodic_reservation_show(EditorPeriodicReservation *this) {
  GObject *periodic_reservation =
      gtk_builder_get_object(this->ui, "periodic-reservation");

  GtkWidget *new = gtk_button_new_with_label("Nowy");
  g_signal_connect(G_OBJECT(new), "clicked", G_CALLBACK(on_edit),
                   prepare_edit(this, true, 0));
  gtk_box_pack_end(GTK_BOX(periodic_reservation), new, 0, 0, 0);

  ID max = repo_len(this->repo, TablePeriodicReservation);
  for (ID i = 0; i < max; i++) {
    PeriodicReservation r;
    repo_get(this->repo, TablePeriodicReservation, i, &r);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

    char *name = describe_periodic_reservation(this->repo, &r);
    GtkWidget *label = gtk_label_new(name);
    free(name);
    gtk_box_pack_start(GTK_BOX(box), label, 1, 0, 0);

    GtkWidget *edit = gtk_button_new_with_label("Edytuj");
    g_signal_connect(G_OBJECT(edit), "clicked", G_CALLBACK(on_edit),
                     prepare_edit(this, false, i));
    gtk_box_pack_start(GTK_BOX(box), edit, 0, 0, 0);

    GtkWidget *del = gtk_button_new_with_label("Usuń");
    g_signal_connect(G_OBJECT(del), "clicked", G_CALLBACK(on_del),
                     prepare_del(this, i));
    gtk_box_pack_start(GTK_BOX(box), del, 0, 0, 0);

    gtk_box_pack_start(GTK_BOX(periodic_reservation), box, 0, 0, 0);
  }
}
