#pragma once
#include <stdbool.h>
#include <gtk/gtk.h>

void bug(char *msg);
char* basedir();
GtkFileFilter* file_filter();
void apply_css(GtkWidget *obj, char *css);
char* temp_file();
bool copy_file(char *src, char *dest);
