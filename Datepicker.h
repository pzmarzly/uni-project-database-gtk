#include "Repo.h"
#include <gtk/gtk.h>

typedef void (*datepicker_callback)(Timestamp, void *);

typedef struct Datepicker Datepicker;
Datepicker *datepicker_new(GtkButton *button, Timestamp initial);
void datepicker_set_callback(Datepicker *this, datepicker_callback update,
                             void *user_data);
Timestamp datepicker_read(Datepicker *this);
