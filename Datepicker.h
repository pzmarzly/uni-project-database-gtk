#include "Repo.h"
#include <gtk/gtk.h>

typedef struct Datepicker Datepicker;
Datepicker *datepicker_new(GtkButton *button, Timestamp initial,
                           void (*update)(Timestamp, void *), void *user_data);
Timestamp datepicker_read(Datepicker *this);
