#include "RepoData.h"
#include "Utils.h"

Timestamp timestamp_now() {
  GDateTime *time = g_date_time_new_now_utc();
  Timestamp ret = g_date_time_to_unix(time);
  g_date_time_unref(time);
  return ret;
}

Timestamp timestamp_midnight(Timestamp timestamp) {
  GDateTime *time = g_date_time_new_from_unix_utc(timestamp);
  int year, month, day;
  g_date_time_get_ymd(time, &year, &month, &day);
  g_date_time_unref(time);

  GTimeZone *tz = g_time_zone_new_utc();
  time = g_date_time_new(tz, year, month, day, 0, 0, 0);
  Timestamp ret = g_date_time_to_unix(time);
  g_date_time_unref(time);

  return ret;
}

Timestamp timestamp_today() { return timestamp_midnight(timestamp_now()); }

GtkWidget *equipment_icon(EquipmentType type, unsigned size) {
  char *path = program_dir(32);
  if (type == Projector) {
    strcat(path, "icons/image-projector.png");
  } else if (type == Laptop) {
    strcat(path, "icons/laptop.png");
  } else if (type == Whiteboard) {
    strcat(path, "icons/white-board.png");
  } else {
    strcat(path, "icons/question-mark.png");
  }
  GError *error = NULL;
  GdkPixbuf *pixbuf =
      gdk_pixbuf_new_from_file_at_size(path, size, size, &error);
  free(path);
  // If pixbuf is NULL, gtk_image_new_from_pixbuf will handle it correctly.
  return gtk_image_new_from_pixbuf(pixbuf);
}

char *removal_window_title(TableID type) {
  if (type == TableEquipment)
    return "Usuwanie sprzętu - WeźMnie";
  if (type == TablePeriodicReservation)
    return "Usuwanie rezerwacji cyklicznej - WeźMnie";
  if (type == TableOneTimeReservation)
    return "Usuwanie rezerwacji jednorazowej - WeźMnie";
  return "Usuwanie - WeźMnie";
}

char *editing_window_title(TableID type) {
  if (type == TableEquipment)
    return "Edycja sprzętu - WeźMnie";
  if (type == TablePeriodicReservation)
    return "Edycja rezerwacji cyklicznej - WeźMnie";
  if (type == TableOneTimeReservation)
    return "Edycja rezerwacji jednorazowej - WeźMnie";
  return "Edycja - WeźMnie";
}

char *day_str(Day day) {
  if (day == Monday)
    return "pon";
  if (day == Tuesday)
    return "wt";
  if (day == Wednesday)
    return "śr";
  if (day == Thursday)
    return "czw";
  if (day == Friday)
    return "pt";
  if (day == Saturday)
    return "sob";
  if (day == Sunday)
    return "nie";
  return "BŁĄD";
}

char *hm_str(HourAndMinutes hm) {
  char *ret = malloc(6);
  sprintf(ret, "%02d:%02d", hm / 60, hm % 60);
  return ret;
}

HourAndMinutes hm_parse(const char *str) {
  unsigned char h, m;
  if (sscanf(str, "%hhu:%hhu", &h, &m) != 2)
    return HM_INVALID;
  return h * 60 + m;
}

char *timestamp_day_str(Timestamp timestamp) {
  GDateTime *time = g_date_time_new_from_unix_utc(timestamp);
  if (time == NULL) {
    printf("Niepoprawna data\n");
    return g_strdup("BŁĄD");
  }
  return g_date_time_format(time, "%d. %b %Y");
}

char *describe_periodic_reservation(PeriodicReservation *r) {
  char *ret = malloc(1024);

  char *start = hm_str(r->start);
  char *end = hm_str(r->end);
  char *since = timestamp_day_str(r->active_since);
  char *until = timestamp_day_str(r->active_until);
  sprintf(ret, "%s %s-%s (od %s do %s)", day_str(r->day), start, end, since,
          until); // TODO: item
  free(until);
  free(since);
  free(end);
  free(start);

  return ret;
}

char *describe_one_time_reservation(OneTimeReservation *r) {
  char *ret = malloc(1024);

  char *start = timestamp_day_str(r->start);
  char *end = timestamp_day_str(r->end);
  sprintf(ret, "%s od %s do %s", "ITEM", start, end); // TODO: item
  free(end);
  free(start);

  return ret;
}
