#pragma once
#include <gtk/gtk.h>
#include <stdbool.h>

_Noreturn void error(char *msg);
_Noreturn void bug(char *msg);

// File handling helper functions
bool file_exists(const char *path);
char *program_dir(int extra_bytes);
char *temp_file();
bool copy_file(char *src, char *dest);

// GTK+ shorthard functions
GtkBuilder *get_builder(char *name);
GtkFileFilter *file_filter();
void apply_css(GtkWidget *obj, char *css);
void remove_all_gtk_children(GtkContainer *container);
