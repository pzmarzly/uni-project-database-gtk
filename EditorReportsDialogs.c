#include "EditorReportsDialogs.h"
#include "RepoData.h"
#include "RepoLogic.h"
#include "Utils.h"
#include <stdio.h>
#include <gtk/gtk.h>

void dialog_week_summary(Repo *repo, Timestamp week_start) {
    GtkBuilder *ui = get_builder("EditorReportsDialogsWeekSummary.glade");
    GObject *dialog = gtk_builder_get_object(ui, "dialog");
    gtk_dialog_add_buttons(GTK_DIALOG(dialog), "OK", GTK_RESPONSE_YES, NULL);

    GObject *list = gtk_builder_get_object(ui, "list");

    Timestamp week_end = timestamp_week_start(week_start + 8 * 24 * 60 * 60);
    OneTimeReservation *ot_list;
    int ot_list_len = reservations_for_time_period(repo, week_start, week_end, &ot_list, INVALID_ID);
    printf("%d %d %d\n", ot_list_len, week_end, week_start);

    for (int i = 0; i < ot_list_len; i++) {
        GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

        char *name = describe_one_time_reservation(repo, ot_list + i);
        GtkWidget *label = gtk_label_new(name);
        free(name);
        gtk_box_pack_start(GTK_BOX(box), label, 1, 0, 0);

        gtk_box_pack_start(GTK_BOX(list), box, 0, 0, 0);
    }

    gtk_widget_show_all(GTK_WIDGET(list));
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(GTK_WIDGET(dialog));
}
void dialog_available_summary(Repo *repo, Timestamp moment) {
    printf("A\n");
}
void dialog_availability_ranking(Repo *repo) {
    printf("A\n");
}
