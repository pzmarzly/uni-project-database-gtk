#pragma once
#include <gtk/gtk.h>

void bug(char *msg);
char* basedir();
GtkFileFilter* file_filter();
void apply_css(GtkWidget *obj, char *css);
