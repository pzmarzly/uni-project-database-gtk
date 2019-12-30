// Paweł Zmarzły 314569
// Projekt, grupa MPi śr 12-14

#include <gtk/gtk.h>
#include "Welcome.h"

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    Welcome* rs = welcome_new();
    welcome_set_quit_on_destroy(rs, true);
    welcome_run(rs);

    gtk_main();
    return 0;
}
