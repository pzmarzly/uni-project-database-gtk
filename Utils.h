#pragma once
#include <gtk/gtk.h>
#include <stdbool.h>

// File handling helper functions
_Noreturn void bug(char *msg);
char *program_dir();
char *temp_file();
bool copy_file(char *src, char *dest);

// GTK+ shorthard functions
GtkBuilder *get_builder(char *name);
GtkFileFilter *file_filter();
void apply_css(GtkWidget *obj, char *css);
void remove_all_gtk_children(GtkContainer *container);
