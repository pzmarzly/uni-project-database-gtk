#include "Datepicker.h"
#include "RepoData.h"
#include "dialog/Dialogs.h"
#include <stdlib.h>

struct Datepicker {
  Timestamp current;
  datepicker_callback update;
  void *user_data;
};

static void update_text(GtkButton *button, Timestamp current) {
  char *date = timestamp_day_str(current);
  gtk_button_set_label(button, date);
  free(date);
}

static void on_click(GtkWidget *sender, gpointer user_data) {
  Datepicker *this = (Datepicker *)user_data;

  PreparedEditDialog d = dialog_edit("Wybierz datę");
  GtkBuilder *ui = d.ui;
  GtkDialog *dialog = d.dialog;
  GtkGrid *grid = GTK_GRID(gtk_builder_get_object(ui, "grid"));

  GtkWidget *calendar_label = GTK_WIDGET(gtk_label_new("Data:"));
  gtk_grid_attach(grid, calendar_label, 0, 0, 1, 1);

  GDateTime *time = g_date_time_new_from_unix_utc(this->current);
  if (time == NULL)
    time = g_date_time_new_from_unix_utc(timestamp_now());
  int year, month, day;
  g_date_time_get_ymd(time, &year, &month, &day);

  GtkCalendar *calendar = GTK_CALENDAR(gtk_calendar_new());
  gtk_calendar_select_month(calendar, month - 1, year);
  gtk_calendar_select_day(calendar, day);
  g_date_time_unref(time);
  gtk_grid_attach(grid, GTK_WIDGET(calendar), 0, 1, 1, 1);
  gtk_widget_set_hexpand(GTK_WIDGET(calendar), true);
  gtk_widget_set_vexpand(GTK_WIDGET(calendar), true);

  gtk_widget_show_all(GTK_WIDGET(dialog));

  int result = gtk_dialog_run(dialog);
  if (result == GTK_RESPONSE_OK) {
    gtk_calendar_get_date(calendar, (unsigned *)&year, (unsigned *)&month,
                          (unsigned *)&day);
    GTimeZone *tz = g_time_zone_new_utc();
    time = g_date_time_new(tz, year, month + 1, day, 0, 0, 0);

    Timestamp ts = g_date_time_to_unix(time);
    this->current = ts;
    if (this->update != NULL)
      this->update(ts, user_data);
    update_text(GTK_BUTTON(sender), ts);
  }
  gtk_widget_destroy(GTK_WIDGET(dialog));
}

Datepicker *datepicker_new(GtkButton *button, Timestamp initial) {
  update_text(button, initial);
  Datepicker *this = malloc(sizeof(Datepicker));
  this->current = initial;
  this->update = NULL;
  this->user_data = NULL;
  g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(on_click), this);
  return this;
}

void datepicker_set_callback(Datepicker *this, datepicker_callback update,
                             void *user_data) {
  this->update = update;
  this->user_data = user_data;
}

Timestamp datepicker_read(Datepicker *this) { return this->current; }
