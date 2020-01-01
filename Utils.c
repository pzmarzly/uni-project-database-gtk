#include "Utils.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <gtk/gtk.h>

void bug(char *msg) {
    printf("BUG: %s\n", msg);
    abort();
}

char* basedir() {
    GError* error = NULL;
    char *path = g_file_read_link("/proc/self/exe", &error);
    *(strrchr(path, '/')) = '\0';
    return path;
}

GtkFileFilter* file_filter() {
    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "Bazy danych (*.db)");
    gtk_file_filter_add_pattern(filter, "*.db");
    return filter;
}

void apply_css(GtkWidget *obj, char *css) {
    GtkCssProvider *prov = gtk_css_provider_new();
    gtk_css_provider_load_from_data(prov, css, strlen(css), NULL);
    GdkScreen *screen = gtk_widget_get_screen(obj);
    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(prov),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}

char* temp_file() {
    GError *error = NULL;
    GFileIOStream *stream = NULL;
    GFile *file = g_file_new_tmp(NULL, &stream, &error);
    if (file == NULL) return NULL;
    if (stream != NULL) g_io_stream_close(G_IO_STREAM(stream), NULL, &error);
    return g_file_get_path(file);
}

bool copy_file(char *src, char *dest) {
    GError *error = NULL;
    GFile *src_file = g_file_new_for_path(src);
    GFile *dest_file = g_file_new_for_path(dest);
    return g_file_copy(src_file, dest_file, G_FILE_COPY_OVERWRITE, NULL, NULL, NULL, &error);
}
