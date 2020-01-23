#include "Utils.h"
#include "dialog/Dialogs.h"
#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void error(char *msg) {
  printf("ERROR: %s\n", msg);
  dialog_info("Błąd krytyczny", msg);
  exit(1);
}

_Noreturn void bug(char *msg) {
  printf("BUG: %s\n", msg);
  exit(1);
}

bool file_exists(const char *path) {
  FILE *f = fopen(path, "rb");
  bool exists = f != NULL;
  if (exists)
    fclose(f);
  return exists;
}

char *program_dir(int extra_bytes) {
  GError *error = NULL;
  // The line below can fail if procfs is not
  // mounted, but in that case the user probably
  // knows what they're doing.
  char *path = g_file_read_link("/proc/self/exe", &error);
  // Path to the executable contains a slash
  // on any sane Unix system.
  *(strrchr(path, '/') + 1) = '\0';
  // Make a copy.
  char *ret = malloc(strlen(path) + extra_bytes + 1);
  strcpy(ret, path);
  free(path);
  return ret;
}

char *temp_file() {
  GError *error = NULL;
  GFileIOStream *stream = NULL;
  GFile *file = g_file_new_tmp(NULL, &stream, &error);
  if (file == NULL)
    return NULL;
  if (stream != NULL)
    g_io_stream_close(G_IO_STREAM(stream), NULL, &error);
  char *path = g_file_get_path(file);
  g_object_unref(file);
  return path;
}

bool copy_file(char *src, char *dest) {
  GError *error = NULL;
  GFile *src_file = g_file_new_for_path(src);
  GFile *dest_file = g_file_new_for_path(dest);
  bool success = g_file_copy(src_file, dest_file, G_FILE_COPY_OVERWRITE, NULL,
                             NULL, NULL, &error);
  g_object_unref(src_file);
  g_object_unref(dest_file);
  return success;
}

GtkBuilder *get_builder(char *name) {
  char *glade = strcat(program_dir(strlen(name) + 1), name);
  GtkBuilder *ui = gtk_builder_new_from_file(glade);
  free(glade);
  return ui;
}

GtkFileFilter *file_filter() {
  GtkFileFilter *filter = gtk_file_filter_new();
  gtk_file_filter_set_name(filter, "Bazy danych (*.db)");
  gtk_file_filter_add_pattern(filter, "*.db");
  return filter;
}

void apply_css(GtkWidget *obj, char *css) {
  GtkCssProvider *prov = gtk_css_provider_new();
  gtk_css_provider_load_from_data(prov, css, strlen(css), NULL);
  GdkScreen *screen = gtk_widget_get_screen(obj);
  gtk_style_context_add_provider_for_screen(
      screen, GTK_STYLE_PROVIDER(prov),
      GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}

// Source: https://stackoverflow.com/a/9193176
void remove_all_gtk_children(GtkContainer *container) {
  GList *children = gtk_container_get_children(container);
  for (GList *i = children; i != NULL; i = g_list_next(i))
    gtk_widget_destroy(GTK_WIDGET(i->data));
  g_list_free(children);
}
