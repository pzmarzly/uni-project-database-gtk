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
    GtkBuilder *ui;
};

EditorEquipment* editor_equipment_new(Repo *repo, GtkBuilder *ui) {
    EditorEquipment *eq = malloc(sizeof(EditorEquipment));
    eq->repo = repo;
    eq->ui = ui;
    return eq;
}

void equipment_refresh(EditorEquipment *eq) {
    GObject *equipment = gtk_builder_get_object(eq->ui, "equipment");
    GList *children = gtk_container_get_children(GTK_CONTAINER(equipment));
    for(GList *i = children; i != NULL; i = g_list_next(i))
        gtk_widget_destroy(GTK_WIDGET(i->data));
    g_list_free(children);

    editor_equipment_show(eq);
    gtk_widget_show_all(GTK_WIDGET(equipment));
}

typedef struct {
    EditorEquipment *eq;
    bool empty;
    ID id;
} EditRequest;

static EditRequest* prepare_edit(EditorEquipment *eq, bool empty, ID id) {
    EditRequest *req = malloc(sizeof(EditRequest));
    req->eq = eq;
    req->empty = empty;
    req->id = id;
    return req;
}

static void on_edit(GtkWidget *sender, gpointer user_data) {
    (void)sender;
    EditRequest *req = (EditRequest*)user_data;

    Equipment e = {
        .name = "",
        .type = Other,
        .description = 0,
    };
    char *desc = g_strdup("");
    if (!req->empty) {
        if (!repo_get(req->eq->repo, TableEquipment, req->id, &e)) {
            printf("Cannot read %u from EditorEquipment.\n", req->id);
            return;
        }
        if (!repo_string_load(req->eq->repo, e.description, &desc)) {
            printf("Cannot read %u from String.\n", e.description);
            return;
        }
    } else {
        req->id = repo_len(req->eq->repo, TableEquipment);
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
    free(desc);

    int result = gtk_dialog_run(GTK_DIALOG(dialog));
    if (result == GTK_RESPONSE_OK) {
        const char *name_str = gtk_entry_get_text(GTK_ENTRY(name));
        strcpy(e.name, name_str);

        e.type = gtk_combo_box_get_active(GTK_COMBO_BOX(type));

        GtkTextIter start, end;
        gtk_text_buffer_get_bounds(buf, &start, &end);
        desc = gtk_text_buffer_get_text(buf, &start, &end, FALSE);

        if (!req->empty)
            repo_string_remove(req->eq->repo, e.description);
        e.description = repo_string_save(req->eq->repo, &desc);
        repo_set(req->eq->repo, TableEquipment, req->id, &e);
        equipment_refresh(req->eq);
    }
    gtk_widget_destroy(GTK_WIDGET(dialog));
}

typedef struct {
    EditorEquipment *eq;
    ID id;
} DelRequest;

static DelRequest* prepare_del(EditorEquipment *eq, ID id) {
    DelRequest *req = malloc(sizeof(DelRequest));
    req->eq = eq;
    req->id = id;
    return req;
}

static void on_del(GtkWidget *sender, gpointer user_data) {
    (void)sender;
    DelRequest *req = (DelRequest*)user_data;

    Equipment e;
    if (!repo_get(req->eq->repo, TableEquipment, req->id, &e)) {
        printf("Cannot read %u from EditorEquipment.\n", req->id);
        return;
    }

    char *glade = strcat(basedir(), "/EditorEquipmentDel.glade");
    GtkBuilder *ui = gtk_builder_new_from_file(glade);
    free(glade);

    GObject *dialog = gtk_builder_get_object(ui, "dialog");
    gtk_dialog_add_buttons(
        GTK_DIALOG(dialog),
        "Nie", GTK_RESPONSE_NO,
        "Tak", GTK_RESPONSE_YES,
        NULL
    );

    GObject *label = gtk_builder_get_object(ui, "label");
    char *text = malloc(128);
    strcpy(text, "Czy na pewno chcesz usunąć ");
    strcat(text, e.name);
    strcat(text, "?");
    gtk_label_set_text(GTK_LABEL(label), text);
    free(text);

    int result = gtk_dialog_run(GTK_DIALOG(dialog));
    if (result == GTK_RESPONSE_YES) {
        repo_string_remove(req->eq->repo, e.description);
        repo_del(req->eq->repo, TableEquipment, req->id);
        equipment_refresh(req->eq);
    }
    gtk_widget_destroy(GTK_WIDGET(dialog));
}

void editor_equipment_show(EditorEquipment *eq) {
    GObject *equipment = gtk_builder_get_object(eq->ui, "equipment");

    GtkWidget *new = gtk_button_new_with_label("Nowy");
    g_signal_connect(G_OBJECT(new), "clicked", G_CALLBACK(on_edit), prepare_edit(eq, true, 0));
    gtk_box_pack_end(GTK_BOX(equipment), new, 0, 0, 0);

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
        g_signal_connect(G_OBJECT(edit), "clicked", G_CALLBACK(on_edit), prepare_edit(eq, false, i));
        gtk_box_pack_start(GTK_BOX(box), edit, 0, 0, 0);

        GtkWidget *del = gtk_button_new_with_label("Usuń");
        g_signal_connect(G_OBJECT(del), "clicked", G_CALLBACK(on_del), prepare_del(eq, i));
        gtk_box_pack_start(GTK_BOX(box), del, 0, 0, 0);

        gtk_box_pack_start(GTK_BOX(equipment), box, 0, 0, 0);
    }
}
