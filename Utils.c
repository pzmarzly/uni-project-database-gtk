#include "Utils.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <gtk/gtk.h>

char* basedir() {
    GError* error = NULL;
    char *path = g_file_read_link("/proc/self/exe", &error);
    *(strrchr(path, '/')) = '\0';
    return path;
}

GtkFileFilter* file_filter() {
    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_add_pattern(filter, "*.db");
    return filter;
}
