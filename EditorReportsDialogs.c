#include "EditorReportsDialogs.h"
#include "RepoData.h"
#include "RepoLogic.h"
#include "Utils.h"
#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>

static int one_time_chronological_order(const void *ot1, const void *ot2) {
  OneTimeReservation *a = (OneTimeReservation *)ot1;
  OneTimeReservation *b = (OneTimeReservation *)ot2;
  if (a->start < b->start)
    return -1;
  if (a->start == b->start)
    return 0;
  return 1;
}

void dialog_week_summary(Repo *repo, Timestamp week_start) {
  Timestamp week_end = timestamp_week_start(week_start + 8 * 24 * 60 * 60);

  GtkTextBuffer *buf = gtk_text_buffer_new(NULL);
  gtk_text_buffer_insert_at_cursor(buf, "Rezerwacje w tygodniu ", -1);
  char *week_start_str = timestamp_day_str(week_start);
  char *week_end_str = timestamp_day_str(week_end - 1);
  gtk_text_buffer_insert_at_cursor(buf, week_start_str, -1);
  gtk_text_buffer_insert_at_cursor(buf, " - ", -1);
  gtk_text_buffer_insert_at_cursor(buf, week_end_str, -1);
  gtk_text_buffer_insert_at_cursor(buf, ":\n\n", -1);
  free(week_start_str);
  free(week_end_str);

  OneTimeReservation *ot_list;
  ID ot_list_len = reservations_for_time_period(
      repo, week_start, week_end, &ot_list, INVALID_ID, INVALID_ID);
  qsort(ot_list, ot_list_len, sizeof(OneTimeReservation),
        one_time_chronological_order);

  GtkBuilder *ui = get_builder("EditorReportsDialogs.glade");
  GObject *dialog = gtk_builder_get_object(ui, "dialog");
  gtk_dialog_add_buttons(GTK_DIALOG(dialog), "OK", GTK_RESPONSE_YES, NULL);

  GObject *list = gtk_builder_get_object(ui, "list");
  for (ID i = 0; i < ot_list_len; i++) {
    char *name = describe_one_time_reservation(repo, ot_list + i);
    gtk_text_buffer_insert_at_cursor(buf, name, -1);
    gtk_text_buffer_insert_at_cursor(buf, "\n", -1);
    free(name);
  }
  free(ot_list);
  GtkWidget *text_view = gtk_text_view_new_with_buffer(buf);
  gtk_box_pack_start(GTK_BOX(list), text_view, 1, 1, 0);

  gtk_widget_show_all(GTK_WIDGET(list));
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(GTK_WIDGET(dialog));
}

void dialog_available_summary(Repo *repo, Timestamp moment) {
  GtkTextBuffer *buf = gtk_text_buffer_new(NULL);
  gtk_text_buffer_insert_at_cursor(buf, "Lista przedmiotów dostępnych ", -1);
  char *day = timestamp_day_str(moment);
  char *hm = hm_str(timestamp_to_hm(moment));
  gtk_text_buffer_insert_at_cursor(buf, day, -1);
  gtk_text_buffer_insert_at_cursor(buf, " ", -1);
  gtk_text_buffer_insert_at_cursor(buf, hm, -1);
  gtk_text_buffer_insert_at_cursor(buf, ":\n\n", -1);
  free(day);
  free(hm);

  // We create a fake OneTimeReservation and search
  // for available items.
  OneTimeReservation ot = {
      .item = INVALID_ID,
      .type = Reservation,
      .start = moment,
      .end = moment + 1,
      .description = INVALID_ID,
  };
  ID eq_len = repo_len(repo, TableEquipment);
  for (ID i = 0; i < eq_len; i++) {
    if (!one_time_can_have_equipment_attached(repo, &ot, INVALID_ID, i))
      continue;

    Equipment eq;
    repo_get(repo, TableEquipment, i, &eq);
    gtk_text_buffer_insert_at_cursor(buf, eq.name, -1);
    gtk_text_buffer_insert_at_cursor(buf, "\n", -1);
  }

  GtkBuilder *ui = get_builder("EditorReportsDialogs.glade");
  GObject *dialog = gtk_builder_get_object(ui, "dialog");
  gtk_dialog_add_buttons(GTK_DIALOG(dialog), "OK", GTK_RESPONSE_YES, NULL);

  GObject *list = gtk_builder_get_object(ui, "list");

  GtkWidget *text_view = gtk_text_view_new_with_buffer(buf);
  gtk_box_pack_start(GTK_BOX(list), text_view, 1, 1, 0);

  gtk_widget_show_all(GTK_WIDGET(list));
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(GTK_WIDGET(dialog));
}

typedef struct {
  Equipment eq;
  float hours;
} EquipmentStats;

static int equipment_usage_order(const void *s1, const void *s2) {
  EquipmentStats *a = (EquipmentStats *)s1;
  EquipmentStats *b = (EquipmentStats *)s2;
  if (a->hours < b->hours)
    return 1;
  if (a->hours == b->hours)
    return 0;
  return -1;
}

void dialog_availability_ranking(Repo *repo) {
  Timestamp start = repo_get_semester_start(repo);
  Timestamp end = repo_get_semester_end(repo);

  ID eq_list_len = repo_len(repo, TableEquipment);
  EquipmentStats *eq_list = (EquipmentStats *)scalloc(eq_list_len, sizeof(EquipmentStats));
  for (ID i = 0; i < eq_list_len; i++) {
    repo_get(repo, TableEquipment, i, &eq_list[i].eq);
    OneTimeReservation *ot_list;
    ID ot_list_len =
        reservations_for_time_period(repo, start, end, &ot_list, i, INVALID_ID);
    for (ID j = 0; j < ot_list_len; j++) {
      eq_list[i].hours +=
          (float)(ot_list[j].end - ot_list[j].start) / (60 * 60);
    }
    free(ot_list);
  }
  qsort(eq_list, eq_list_len, sizeof(EquipmentStats), equipment_usage_order);

  GtkBuilder *ui = get_builder("EditorReportsDialogs.glade");
  GObject *dialog = gtk_builder_get_object(ui, "dialog");
  gtk_dialog_add_buttons(GTK_DIALOG(dialog), "OK", GTK_RESPONSE_YES, NULL);

  GObject *list = gtk_builder_get_object(ui, "list");

  GtkTextBuffer *buf = gtk_text_buffer_new(NULL);
  gtk_text_buffer_insert_at_cursor(
      buf, "Łączna ilość godzin, przez które sprzęt był rezerwowany:\n\n", -1);
  for (ID i = 0; i < eq_list_len; i++) {
    gtk_text_buffer_insert_at_cursor(buf, eq_list[i].eq.name, -1);
    gtk_text_buffer_insert_at_cursor(buf, " - ", -1);
    char hours[128];
    sprintf(hours, "%.02f", eq_list[i].hours);
    gtk_text_buffer_insert_at_cursor(buf, hours, -1);
    gtk_text_buffer_insert_at_cursor(buf, " godzin\n", -1);
  }
  free(eq_list);
  GtkWidget *text_view = gtk_text_view_new_with_buffer(buf);
  gtk_box_pack_start(GTK_BOX(list), text_view, 1, 1, 0);

  gtk_widget_show_all(GTK_WIDGET(list));
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(GTK_WIDGET(dialog));
}
