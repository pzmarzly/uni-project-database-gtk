#include "EditorRemovalDialog.h"
#include <gtk/gtk.h>
#include "RepoData.h"
#include "Utils.h"

bool editor_removal_dialog(TableID type, char *element_name) {
    GtkBuilder *ui = get_builder("EditorRemovalDialog.glade");
    GObject *dialog = gtk_builder_get_object(ui, "dialog");
    gtk_dialog_add_buttons(
        GTK_DIALOG(dialog),
        "Nie", GTK_RESPONSE_NO,
        "Tak", GTK_RESPONSE_YES,
        NULL
    );

    char title[128];
    strcpy(title, "Usuwanie ");
    strcat(title, removal_and_editing_text(type));
    strcat(title, " - WeźMnie");
    gtk_window_set_title(GTK_WINDOW(dialog), title);

    GObject *label = gtk_builder_get_object(ui, "label");
    char text[128];
    strcpy(text, "Czy na pewno chcesz usunąć ");
    strcat(text, element_name);
    strcat(text, "?");
    gtk_label_set_text(GTK_LABEL(label), text);

    int result = gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(GTK_WIDGET(dialog));
    return result == GTK_RESPONSE_YES;
}
