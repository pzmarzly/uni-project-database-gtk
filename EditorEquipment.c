#include "EditorEquipment.h"
#include "Editor.h"
#include "EditorDialogs.h"
#include "Repo.h"
#include "RepoData.h"
#include "RepoString.h"
#include "Utils.h"
#include <gtk/gtk.h>
#include <stdbool.h>

struct EditorEquipment {
  Repo *repo;
  GtkBuilder *ui;
};

EditorEquipment *editor_equipment_new(Repo *repo, GtkBuilder *ui) {
  EditorEquipment *this = malloc(sizeof(EditorEquipment));
  this->repo = repo;
  this->ui = ui;
  return this;
}

void equipment_refresh(EditorEquipment *this) {
  GObject *equipment = gtk_builder_get_object(this->ui, "equipment");
  remove_all_gtk_children(GTK_CONTAINER(equipment));
  editor_equipment_show(this);
  gtk_widget_show_all(GTK_WIDGET(equipment));
}

typedef struct {
  EditorEquipment *this;
  bool empty;
  ID id;
} EditRequest;

static EditRequest *prepare_edit(EditorEquipment *this, bool empty, ID id) {
  EditRequest *req = malloc(sizeof(EditRequest));
  req->this = this;
  req->empty = empty;
  req->id = id;
  return req;
}

static void on_edit(GtkWidget *sender, gpointer user_data) {
  (void)sender;
  EditRequest *req = (EditRequest *)user_data;

  Equipment e = {
      .name = "",
      .type = Other,
      .description = 0,
  };
  char *desc;
  if (!req->empty) {
    repo_get(req->this->repo, TableEquipment, req->id, &e);
    repo_string_get(req->this->repo, e.description, &desc);
  } else {
    req->id = repo_len(req->this->repo, TableEquipment);
    desc = g_strdup("");
    e.description = repo_string_len(req->this->repo);
  }

  PreparedEditDialog d = editor_edit_dialog_prepare(TableEquipment);
  GtkBuilder *ui = d.ui;
  GtkDialog *dialog = d.dialog;
  GtkGrid *grid = GTK_GRID(gtk_builder_get_object(ui, "grid"));

  GtkWidget *name_label = GTK_WIDGET(gtk_label_new("Nazwa:"));
  gtk_grid_attach(grid, name_label, 0, 0, 1, 1);
  GtkWidget *type_label = GTK_WIDGET(gtk_label_new("Typ:"));
  gtk_grid_attach(grid, type_label, 0, 1, 1, 1);
  GtkWidget *description_label = GTK_WIDGET(gtk_label_new("Opis:"));
  gtk_grid_attach(grid, description_label, 0, 2, 1, 1);

  GtkEntry *name_entry = GTK_ENTRY(gtk_entry_new());
  gtk_entry_set_text(name_entry, e.name);
  gtk_entry_set_max_length(name_entry, 30);
  gtk_grid_attach(grid, GTK_WIDGET(name_entry), 1, 0, 1, 1);

  GtkComboBox *type_combo_box = GTK_COMBO_BOX(gtk_combo_box_text_new());
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(type_combo_box), NULL,
                            "Rzutnik");
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(type_combo_box), NULL, "Laptop");
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(type_combo_box), NULL, "Ekran");
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(type_combo_box), NULL,
                            "Inny sprzęt");
  gtk_combo_box_set_active(GTK_COMBO_BOX(type_combo_box), e.type);
  gtk_grid_attach(grid, GTK_WIDGET(type_combo_box), 1, 1, 1, 1);

  GtkTextView *description_text_view = GTK_TEXT_VIEW(gtk_text_view_new());
  GtkTextBuffer *buf =
      gtk_text_view_get_buffer(GTK_TEXT_VIEW(description_text_view));
  gtk_text_buffer_insert_at_cursor(buf, desc, -1);
  free(desc);
  gtk_grid_attach(grid, GTK_WIDGET(description_text_view), 1, 2, 1, 1);
  gtk_widget_set_hexpand(GTK_WIDGET(description_text_view), true);
  gtk_widget_set_vexpand(GTK_WIDGET(description_text_view), true);

  gtk_widget_show_all(GTK_WIDGET(dialog));

  while (true) {
    int result = gtk_dialog_run(dialog);
    if (result != GTK_RESPONSE_OK)
      break;

    const char *name_str = gtk_entry_get_text(name_entry);
    if (strlen(name_str) == 0) {
      validation_error("Pole nazwy nie może być puste");
      continue;
    }
    strcpy(e.name, name_str);

    e.type = gtk_combo_box_get_active(type_combo_box);

    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buf, &start, &end);
    desc = gtk_text_buffer_get_text(buf, &start, &end, FALSE);

    repo_string_set(req->this->repo, e.description, &desc);
    repo_set(req->this->repo, TableEquipment, req->id, &e);
    equipment_refresh(req->this);
    break;
  }
  gtk_widget_destroy(GTK_WIDGET(dialog));
}

typedef struct {
  EditorEquipment *this;
  ID id;
} DelRequest;

static DelRequest *prepare_del(EditorEquipment *this, ID id) {
  DelRequest *req = malloc(sizeof(DelRequest));
  req->this = this;
  req->id = id;
  return req;
}

static void on_del(GtkWidget *sender, gpointer user_data) {
  (void)sender;
  DelRequest *req = (DelRequest *)user_data;

  Equipment e;
  repo_get(req->this->repo, TableEquipment, req->id, &e);
  if (editor_removal_dialog(TableEquipment, e.name)) {
    repo_string_del(req->this->repo, e.description);
    repo_del(req->this->repo, TableEquipment, req->id);
    equipment_refresh(req->this);
  }
}

void editor_equipment_show(EditorEquipment *this) {
  GObject *equipment = gtk_builder_get_object(this->ui, "equipment");

  GtkWidget *new = gtk_button_new_with_label("Nowy");
  g_signal_connect(G_OBJECT(new), "clicked", G_CALLBACK(on_edit),
                   prepare_edit(this, true, 0));
  gtk_box_pack_end(GTK_BOX(equipment), new, 0, 0, 0);

  ID max = repo_len(this->repo, TableEquipment);
  for (ID i = 0; i < max; i++) {
    Equipment e;
    repo_get(this->repo, TableEquipment, i, &e);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

    GtkWidget *image = equipment_icon(e.type, 32);
    gtk_widget_set_margin_start(image, 5);
    gtk_box_pack_start(GTK_BOX(box), image, 0, 0, 0);

    GtkWidget *label = gtk_label_new(e.name);
    gtk_box_pack_start(GTK_BOX(box), label, 1, 0, 0);

    GtkWidget *edit = gtk_button_new_with_label("Edytuj");
    g_signal_connect(G_OBJECT(edit), "clicked", G_CALLBACK(on_edit),
                     prepare_edit(this, false, i));
    gtk_box_pack_start(GTK_BOX(box), edit, 0, 0, 0);

    GtkWidget *del = gtk_button_new_with_label("Usuń");
    g_signal_connect(G_OBJECT(del), "clicked", G_CALLBACK(on_del),
                     prepare_del(this, i));
    gtk_box_pack_start(GTK_BOX(box), del, 0, 0, 0);

    gtk_box_pack_start(GTK_BOX(equipment), box, 0, 0, 0);
  }
}
