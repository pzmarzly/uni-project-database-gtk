#include "Repo.h"
#include <gtk/gtk.h>

typedef struct Datepicker Datepicker;
void datepicker_new(GtkButton *button, Timestamp initial,
                    void (*update)(Timestamp, void *), void *user_data);
