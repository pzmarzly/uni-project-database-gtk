#include "RepoData.h"
#include "Utils.h"
#include <stdlib.h>
#include <string.h>

Timestamp timestamp_now() {
  GDateTime *utc_time = g_date_time_new_now_utc();
  Timestamp ret = g_date_time_to_unix(utc_time);
  g_date_time_unref(utc_time);
  return ret;
}

Timestamp timestamp_midnight(Timestamp timestamp) {
  GDateTime *utc_time = g_date_time_new_from_unix_utc(timestamp);
  int year, month, day;
  g_date_time_get_ymd(utc_time, &year, &month, &day);
  g_date_time_unref(utc_time);

  GTimeZone *tz_utc = g_time_zone_new_utc();
  utc_time = g_date_time_new(tz_utc, year, month, day, 0, 0, 0);
  Timestamp ret = g_date_time_to_unix(utc_time);
  g_date_time_unref(utc_time);
  g_time_zone_unref(tz_utc);

  return ret;
}

Timestamp timestamp_week_start(Timestamp timestamp) {
  while (timestamp_to_day(timestamp_midnight(timestamp)) != Monday) timestamp -= 24 * 60 * 60;
  return timestamp_midnight(timestamp);
}

Timestamp timestamp_today() { return timestamp_midnight(timestamp_now()); }

Timestamp timestamp_add_week(Timestamp timestamp) {
  GDateTime *utc_time = g_date_time_new_from_unix_utc(timestamp);
  GDateTime *utc_time_plus_week = g_date_time_add_weeks(utc_time, 1);
  Timestamp ret = g_date_time_to_unix(utc_time_plus_week);
  g_date_time_unref(utc_time);
  g_date_time_unref(utc_time_plus_week);
  return ret;
}

Day timestamp_to_day(Timestamp timestamp) {
  GDateTime *utc_time = g_date_time_new_from_unix_utc(timestamp);
  GTimeZone *tz_local = g_time_zone_new_local();
  GDateTime *time = g_date_time_to_timezone(utc_time, tz_local);
  // TODO: think about timezones
  int ret = g_date_time_get_day_of_week(time) - 1;
  g_date_time_unref(time);
  g_date_time_unref(utc_time);
  g_time_zone_unref(tz_local);
  return ret;
}

HourAndMinutes timestamp_to_hm(Timestamp timestamp) {
  GDateTime *utc_time = g_date_time_new_from_unix_utc(timestamp);
  GTimeZone *tz_local = g_time_zone_new_local();
  GDateTime *time = g_date_time_to_timezone(utc_time, tz_local);
  int hour = g_date_time_get_hour(time);
  int minute = g_date_time_get_minute(time);
  g_date_time_unref(time);
  g_date_time_unref(utc_time);
  g_time_zone_unref(tz_local);
  return hour * 60 + minute;
}

Timestamp hm_to_timestamp(Timestamp midnight, HourAndMinutes hm) {
  if (midnight != timestamp_midnight(midnight)) {
    warn("internal bug: midnight provided to hm_to_timestamp is not midnight");
    midnight = timestamp_midnight(midnight);
  }
  GDateTime *time = g_date_time_new_from_unix_utc(midnight);
  int year, month, day;
  g_date_time_get_ymd(time, &year, &month, &day);
  g_date_time_unref(time);

  time = g_date_time_new_local(year, month, day, hm / 60, hm % 60, 0);
  Timestamp ret = g_date_time_to_unix(time);
  g_date_time_unref(time);
  return ret;
}

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
  if (sscanf(str, "%hhu:%hhu", &h, &m) != 1)
    return HM_INVALID;
  if (h > 23)
    return HM_INVALID;
  if (m > 59)
    return HM_INVALID;
  return h * 60 + m;
}

char *timestamp_day_str(Timestamp timestamp) {
  GDateTime *time = g_date_time_new_from_unix_utc(timestamp);
  if (time == NULL) {
    warn("invalid date format");
    return g_strdup("BŁĄD");
  }
  return g_date_time_format(time, "%d. %b %Y");
}

char *equipment_str(Repo *repo, ID equipment_id) {
  Equipment e;
  repo_get(repo, TableEquipment, equipment_id, &e);
  return g_strdup(e.name);
}

char *describe_periodic_reservation(Repo *repo, PeriodicReservation *r) {
  char *ret = malloc(1024);

  char *start = hm_str(r->start);
  char *end = hm_str(r->end);
  char *since = timestamp_day_str(r->active_since);
  char *until = timestamp_day_str(r->active_until);
  sprintf(ret, "%s, %s %s-%s (od %s do %s)", equipment_str(repo, r->item),
          day_str(r->day), start, end, since, until);
  free(until);
  free(since);
  free(end);
  free(start);

  return ret;
}

char *describe_one_time_reservation(Repo *repo, OneTimeReservation *r) {
  char *ret = malloc(1024);

  char *day = timestamp_day_str(timestamp_midnight(r->start));
  char *start = hm_str(timestamp_to_hm(r->start));
  char *end = hm_str(timestamp_to_hm(r->end));
  sprintf(ret, "%s, %s od %s do %s", equipment_str(repo, r->item), day, start,
          end);
  free(end);
  free(start);

  return ret;
}
