#include "EditorReportsDialogs.h"
#include "RepoData.h"
#include "RepoLogic.h"
#include "Utils.h"
#include <stdio.h>
#include <gtk/gtk.h>

int one_time_chronological_order(const void* ot1, const void* ot2) {
    OneTimeReservation *a = (OneTimeReservation *)ot1;
    OneTimeReservation *b = (OneTimeReservation *)ot2;
    if (a->start < b->start) return -1;
    if (a->start == b->start) return 0;
    return 1;
}

void dialog_week_summary(Repo *repo, Timestamp week_start) {
    GtkBuilder *ui = get_builder("EditorReportsDialogsWeekSummary.glade");
    GObject *dialog = gtk_builder_get_object(ui, "dialog");
    gtk_dialog_add_buttons(GTK_DIALOG(dialog), "OK", GTK_RESPONSE_YES, NULL);

    GObject *list = gtk_builder_get_object(ui, "list");

    Timestamp week_end = timestamp_week_start(week_start + 8 * 24 * 60 * 60);
    OneTimeReservation *ot_list;
    int ot_list_len = reservations_for_time_period(repo, week_start, week_end, &ot_list, INVALID_ID);

    qsort(ot_list, ot_list_len, sizeof(OneTimeReservation), one_time_chronological_order);

    GtkTextBuffer *buf = gtk_text_buffer_new(NULL);
    for (int i = 0; i < ot_list_len; i++) {
        char *name = describe_one_time_reservation(repo, ot_list + i);
        gtk_text_buffer_insert_at_cursor(buf, name, -1);
        gtk_text_buffer_insert_at_cursor(buf, "\n", -1);
    }
    GtkWidget *text_view = gtk_text_view_new_with_buffer(buf);
    gtk_box_pack_start(GTK_BOX(list), text_view, 1, 1, 0);

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
