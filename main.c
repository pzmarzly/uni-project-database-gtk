// Paweł Zmarzły 314569
// Projekt, grupa MPi śr 12-14

#include <gtk/gtk.h>
#include <stdlib.h>
#include "Welcome.h"
#include "Editor.h"

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    if (argc == 1) {
        Welcome* rs = welcome_new();
        welcome_set_quit_on_destroy(rs, true);
        welcome_run(rs);
        gtk_main();
    } else if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
        printf("WeźMnie - system rezerwacji sprzętu\n");
        printf("Użycie: %s [-v|--version|-h|--help|FILE]\n", argv[0]);
    } else if (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0) {
        printf("1.0.0\n");
    } else {
        FILE *f = fopen(argv[1], "rb");
        bool override = f == NULL;
        if (f != NULL) fclose(f);
        Editor* re = editor_new(argv[1], override);
        editor_set_quit_on_destroy(re, true);
        editor_run(re);
        gtk_main();
    }

    return 0;
}
