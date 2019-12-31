#include "EditorEquipment.h"
#include <stdbool.h>
#include <gtk/gtk.h>
#include "Editor.h"
#include "Repo.h"
#include "RepoData.h"
#include "RepoString.h"
#include "Utils.h"

struct EditorEquipment {
    Repo *repo;
};

EditorEquipment* editor_equipment_new(Repo *repo) {
    EditorEquipment *eq = malloc(sizeof(EditorEquipment));
    eq->repo = repo;
    return eq;
}

typedef struct {
    EditorEquipment *eq;
    ID id;
} EditRequest;

static EditRequest* prepare_edit(EditorEquipment *eq, ID id) {
    EditRequest *req = malloc(sizeof(EditRequest));
    req->eq = eq;
    req->id = id;
    return req;
}

static void on_edit(GtkWidget *sender, gpointer user_data) {
    (void)sender;
    EditRequest *req = (EditRequest*)user_data;

    Equipment e;
    if (!repo_get(req->eq->repo, TableEquipment, req->id, &e)) {
        printf("Cannot read %u from EditorEquipment.\n", req->id);
        return;
    }
    char *desc;
    if (!repo_string_load(req->eq->repo, e.description, &desc)) {
        printf("Cannot read %u from String.\n", e.description);
        return;
    }

    char *glade = strcat(basedir(), "/EditorEquipmentEdit.glade");
    GtkBuilder *ui = gtk_builder_new_from_file(glade);
    free(glade);

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

    int result = gtk_dialog_run(GTK_DIALOG(dialog));
    switch (result)
    {
        case GTK_RESPONSE_OK:
            printf("x\n");
        break;
        default:
            printf("y\n");
        break;
    }
    gtk_widget_destroy(GTK_WIDGET(dialog));
}

void editor_equipment_show(EditorEquipment *eq, GtkBox *target) {
    ID max = repo_len(eq->repo, TableEquipment);
    for (ID i = 0; i < max; i++) {
        Equipment e;
        if (!repo_get(eq->repo, TableEquipment, i, &e)) {
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
        g_signal_connect(G_OBJECT(edit), "clicked", G_CALLBACK(on_edit), prepare_edit(eq, i));
        gtk_box_pack_start(GTK_BOX(box), edit, 0, 0, 0);

        GtkWidget *del = gtk_button_new_with_label("Usuń");
        g_signal_connect(G_OBJECT(del), "clicked", G_CALLBACK(on_edit), prepare_edit(eq, i));
        gtk_box_pack_start(GTK_BOX(box), del, 0, 0, 0);

        gtk_box_pack_start(GTK_BOX(target), box, 0, 0, 0);
    }
}
