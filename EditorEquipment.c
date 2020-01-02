#include "EditorEquipment.h"
#include <stdbool.h>
#include <gtk/gtk.h>
#include "Editor.h"
#include "EditorRemovalDialog.h"
#include "Repo.h"
#include "RepoData.h"
#include "RepoString.h"
#include "Utils.h"

struct EditorEquipment {
    Repo *repo;
    GtkBuilder *ui;
};

EditorEquipment* editor_equipment_new(Repo *repo, GtkBuilder *ui) {
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

static EditRequest* prepare_edit(EditorEquipment *this, bool empty, ID id) {
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
    char *desc = g_strdup("");
    if (!req->empty) {
        if (!repo_get(req->this->repo, TableEquipment, req->id, &e)) {
            printf("Cannot read %u from EditorEquipment.\n", req->id);
            return;
        }
        if (!repo_string_get(req->this->repo, e.description, &desc)) {
            printf("Cannot read string %u.\n", e.description); // TODO: PL
            return;
        }
    } else {
        req->id = repo_len(req->this->repo, TableEquipment);
    }

    GtkBuilder *ui = get_builder("EditorEquipmentEdit.glade");
    GObject *dialog = gtk_builder_get_object(ui, "dialog");
    gtk_dialog_add_buttons(
        GTK_DIALOG(dialog),
        "Anuluj", GTK_RESPONSE_CANCEL,
        "Zapisz", GTK_RESPONSE_OK,
        NULL
    );

    GObject *name = gtk_builder_get_object(ui, "name");
    gtk_entry_set_text(GTK_ENTRY(name), e.name);
    gtk_entry_set_max_length(GTK_ENTRY(name), 30);

    GObject *type = gtk_builder_get_object(ui, "type");
    gtk_combo_box_set_active(GTK_COMBO_BOX(type), e.type);

    GObject *description = gtk_builder_get_object(ui, "description");
    GtkTextBuffer *buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(description));
    gtk_text_buffer_insert_at_cursor(buf, desc, -1);
    free(desc);

    int result = gtk_dialog_run(GTK_DIALOG(dialog));
    if (result == GTK_RESPONSE_OK) {
        const char *name_str = gtk_entry_get_text(GTK_ENTRY(name));
        strcpy(e.name, name_str);

        e.type = gtk_combo_box_get_active(GTK_COMBO_BOX(type));

        GtkTextIter start, end;
        gtk_text_buffer_get_bounds(buf, &start, &end);
        desc = gtk_text_buffer_get_text(buf, &start, &end, FALSE);

        repo_string_set(req->this->repo, e.description, &desc);
        repo_set(req->this->repo, TableEquipment, req->id, &e);
        equipment_refresh(req->this);
    }
    gtk_widget_destroy(GTK_WIDGET(dialog));
}

typedef struct {
    EditorEquipment *this;
    ID id;
} DelRequest;

static DelRequest* prepare_del(EditorEquipment *this, ID id) {
    DelRequest *req = malloc(sizeof(DelRequest));
    req->this = this;
    req->id = id;
    return req;
}

static void on_del(GtkWidget *sender, gpointer user_data) {
    (void)sender;
    DelRequest *req = (DelRequest *)user_data;

    Equipment e;
    if (!repo_get(req->this->repo, TableEquipment, req->id, &e)) {
        printf("Cannot read %u from EditorEquipment.\n", req->id);
        return;
    }
    if (editor_removal_dialog(TableEquipment, e.name)) {
        repo_string_del(req->this->repo, e.description);
        repo_del(req->this->repo, TableEquipment, req->id);
        equipment_refresh(req->this);
    }
}

void editor_equipment_show(EditorEquipment *this) {
    GObject *equipment = gtk_builder_get_object(this->ui, "equipment");

    GtkWidget *new = gtk_button_new_with_label("Nowy");
    g_signal_connect(G_OBJECT(new), "clicked", G_CALLBACK(on_edit), prepare_edit(this, true, 0));
    gtk_box_pack_end(GTK_BOX(equipment), new, 0, 0, 0);

    ID max = repo_len(this->repo, TableEquipment);
    for (ID i = 0; i < max; i++) {
        Equipment e;
        if (!repo_get(this->repo, TableEquipment, i, &e)) {
            printf("Cannot read %u from EditorEquipment.\n", i);
            continue;
        }

        GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

        GtkWidget *image = equipment_icon(e.type, 32);
        gtk_widget_set_margin_start(image, 5);
        gtk_box_pack_start(GTK_BOX(box), image, 0, 0, 0);

        GtkWidget *label = gtk_label_new(e.name);
        gtk_box_pack_start(GTK_BOX(box), label, 1, 0, 0);

        GtkWidget *edit = gtk_button_new_with_label("Edytuj");
        g_signal_connect(G_OBJECT(edit), "clicked", G_CALLBACK(on_edit), prepare_edit(this, false, i));
        gtk_box_pack_start(GTK_BOX(box), edit, 0, 0, 0);

        GtkWidget *del = gtk_button_new_with_label("Usuń");
        g_signal_connect(G_OBJECT(del), "clicked", G_CALLBACK(on_del), prepare_del(this, i));
        gtk_box_pack_start(GTK_BOX(box), del, 0, 0, 0);

        gtk_box_pack_start(GTK_BOX(equipment), box, 0, 0, 0);
    }
}
