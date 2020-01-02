#pragma once
#include <stdbool.h>
#include <gtk/gtk.h>

_Noreturn void bug(char *msg);
char* program_dir();

GtkBuilder *get_builder(char *name);
GtkFileFilter* file_filter();
void apply_css(GtkWidget *obj, char *css);

char* temp_file();
bool copy_file(char *src, char *dest);
