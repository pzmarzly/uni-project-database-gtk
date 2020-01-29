#pragma once
#include <gtk/gtk.h>
#include <stdbool.h>

void *smalloc(size_t bytes);
void *scalloc(size_t amount, size_t size);

_Noreturn void error(char *msg);
_Noreturn void bug(char *msg);
void warn(const char *format, ...);

// File handling helper functions.
bool file_exists(const char *path);
char *program_dir(int extra_bytes);
char *temp_file();
bool copy_file(char *src, char *dest);

// GTK+ shorthard functions.
GtkBuilder *get_builder(char *name);
GtkFileFilter *file_filter();
void apply_css(GtkWidget *obj, char *css);
void remove_all_gtk_children(GtkContainer *container);
