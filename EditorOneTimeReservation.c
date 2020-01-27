#include "EditorOneTimeReservation.h"
#include "Datepicker.h"
#include "Editor.h"
#include "EditorDialogs.h"
#include "Repo.h"
#include "RepoData.h"
#include "RepoString.h"
#include "Utils.h"
#include <stdbool.h>
#include <stdlib.h>

struct EditorOneTimeReservation {
  Repo *repo;
  GtkBuilder *ui;
};

EditorOneTimeReservation *editor_one_time_reservation_new(Repo *repo,
                                                          GtkBuilder *ui) {
  EditorOneTimeReservation *this = malloc(sizeof(EditorOneTimeReservation));
  this->repo = repo;
  this->ui = ui;
  return this;
}

void one_time_reservation_refresh(EditorOneTimeReservation *this) {
  GObject *one_time_reservation =
      gtk_builder_get_object(this->ui, "one-time-reservation");
  remove_all_gtk_children(GTK_CONTAINER(one_time_reservation));
  editor_one_time_reservation_show(this);
  gtk_widget_show_all(GTK_WIDGET(one_time_reservation));
}

typedef struct {
  EditorOneTimeReservation *this;
  bool empty;
  ID id;
} EditRequest;

static EditRequest *prepare_edit(EditorOneTimeReservation *this, bool empty,
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

  OneTimeReservation r = {
      .item = INVALID_ID,
      .type = Reservation,
      .start = 0,
      .end = 0,
      .description = 0,
  };
  char *desc;
  if (!req->empty) {
    repo_get(req->this->repo, TableOneTimeReservation, req->id, &r);
    repo_string_get(req->this->repo, r.description, &desc);
  } else {
    req->id = repo_len(req->this->repo, TableOneTimeReservation);
    desc = g_strdup("");
    r.description = repo_string_len(req->this->repo);
    r.start = timestamp_now();
    r.end = timestamp_now();
  }

  PreparedEditDialog d = editor_edit_dialog_prepare(TableOneTimeReservation);
  GtkBuilder *ui = d.ui;
  GtkDialog *dialog = d.dialog;
  GtkGrid *grid = GTK_GRID(gtk_builder_get_object(ui, "grid"));

  GtkWidget *date_label = GTK_WIDGET(gtk_label_new("Dzień:"));
  gtk_grid_attach(grid, date_label, 0, 0, 1, 1);
  GtkWidget *start_label = GTK_WIDGET(gtk_label_new("Od [HH:MM]:"));
  gtk_grid_attach(grid, start_label, 0, 1, 1, 1);
  GtkWidget *end_label = GTK_WIDGET(gtk_label_new("Do [HH:MM]:"));
  gtk_grid_attach(grid, end_label, 0, 2, 1, 1);
  GtkWidget *description_label = GTK_WIDGET(gtk_label_new("Opis:"));
  gtk_grid_attach(grid, description_label, 0, 3, 1, 1);

  GtkButton *date_button = GTK_BUTTON(gtk_button_new_with_label(""));
  Datepicker *date = datepicker_new(date_button, timestamp_midnight(r.start));
  gtk_grid_attach(grid, GTK_WIDGET(date_button), 1, 0, 1, 1);

  GtkEntry *start_entry = GTK_ENTRY(gtk_entry_new());
  gtk_entry_set_text(start_entry, hm_str(timestamp_to_hm(r.start)));
  gtk_entry_set_max_length(start_entry, 5);
  gtk_grid_attach(grid, GTK_WIDGET(start_entry), 1, 1, 1, 1);

  GtkEntry *end_entry = GTK_ENTRY(gtk_entry_new());
  gtk_entry_set_text(end_entry, hm_str(timestamp_to_hm(r.end)));
  gtk_entry_set_max_length(end_entry, 5);
  gtk_grid_attach(grid, GTK_WIDGET(end_entry), 1, 2, 1, 1);

  GtkTextView *description_text_view = GTK_TEXT_VIEW(gtk_text_view_new());
  GtkTextBuffer *buf =
      gtk_text_view_get_buffer(GTK_TEXT_VIEW(description_text_view));
  gtk_text_buffer_insert_at_cursor(buf, desc, -1);
  free(desc);
  gtk_grid_attach(grid, GTK_WIDGET(description_text_view), 1, 3, 1, 1);
  gtk_widget_set_hexpand(GTK_WIDGET(description_text_view), true);
  gtk_widget_set_vexpand(GTK_WIDGET(description_text_view), true);

  gtk_widget_show_all(GTK_WIDGET(dialog));

  while (true) {
    int result = gtk_dialog_run(dialog);
    if (result != GTK_RESPONSE_OK)
      break;

    Timestamp day = datepicker_read(date);

    const char *start_str = gtk_entry_get_text(start_entry);
    HourAndMinutes start_hm = hm_parse(start_str);
    const char *end_str = gtk_entry_get_text(end_entry);
    HourAndMinutes end_hm = hm_parse(end_str);
    if (start_hm == HM_INVALID || end_hm == HM_INVALID) {
      validation_error("Niepoprawny format godziny!");
      continue;
    }

    r.start = hm_to_timestamp(day, start_hm);
    r.end = hm_to_timestamp(day, end_hm);

    if (r.start >= r.end) {
      validation_error(
          "Godzina końcowa nie może być mniejsza niż godzina początkowa!");
      continue;
    }

    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buf, &start, &end);
    desc = gtk_text_buffer_get_text(buf, &start, &end, FALSE);

    if (!ask_for_item_one_time(&r, req->id, req->this->repo))
      continue;

    repo_string_set(req->this->repo, r.description, &desc);
    repo_set(req->this->repo, TableOneTimeReservation, req->id, &r);
    one_time_reservation_refresh(req->this);
    break;
  }
  gtk_widget_destroy(GTK_WIDGET(dialog));
}

typedef struct {
  EditorOneTimeReservation *this;
  ID id;
} DelRequest;

static DelRequest *prepare_del(EditorOneTimeReservation *this, ID id) {
  DelRequest *req = malloc(sizeof(DelRequest));
  req->this = this;
  req->id = id;
  return req;
}

static void on_del(GtkWidget *sender, gpointer user_data) {
  (void)sender;
  DelRequest *req = (DelRequest *)user_data;

  OneTimeReservation r;
  repo_get(req->this->repo, TableOneTimeReservation, req->id, &r);
  char *name = describe_one_time_reservation(req->this->repo, &r);
  if (editor_removal_dialog(TableOneTimeReservation, name)) {
    repo_string_del(req->this->repo, r.description);
    repo_del(req->this->repo, TableOneTimeReservation, req->id);
    one_time_reservation_refresh(req->this);
  }
  free(name);
}

void editor_one_time_reservation_show(EditorOneTimeReservation *this) {
  GObject *one_time_reservation =
      gtk_builder_get_object(this->ui, "one-time-reservation");

  GtkWidget *new = gtk_button_new_with_label("Nowy");
  g_signal_connect(G_OBJECT(new), "clicked", G_CALLBACK(on_edit),
                   prepare_edit(this, true, 0));
  gtk_box_pack_end(GTK_BOX(one_time_reservation), new, 0, 0, 0);

  ID max = repo_len(this->repo, TableOneTimeReservation);
  for (ID i = 0; i < max; i++) {
    OneTimeReservation r;
    repo_get(this->repo, TableOneTimeReservation, i, &r);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

    char *name = describe_one_time_reservation(this->repo, &r);
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

    gtk_box_pack_start(GTK_BOX(one_time_reservation), box, 0, 0, 0);
  }
}
