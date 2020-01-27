#include "EditorReportsDialogs.h"
#include "Utils.h"
#include <stdio.h>
#include <gtk/gtk.h>

void dialog_week_summary(Repo *repo, Timestamp week_start) {
    GtkBuilder *ui = get_builder("EditorReportsDialogsWeekSummary.glade");
    GObject *dialog = gtk_builder_get_object(ui, "dialog");
    gtk_dialog_add_buttons(GTK_DIALOG(dialog), "Nie", GTK_RESPONSE_NO, "Tak",
                            GTK_RESPONSE_YES, NULL);

    GObject *label = gtk_builder_get_object(ui, "label");
    gtk_label_set_text(GTK_LABEL(label), "a");

    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(GTK_WIDGET(dialog));
}
void dialog_available_summary(Repo *repo, Timestamp moment) {
    printf("A\n");
}
void dialog_availability_ranking(Repo *repo) {
    printf("A\n");
}
