// Paweł Zmarzły 314569
// Projekt, grupa MPi śr 12-14

#include <stdlib.h>
#include <gtk/gtk.h>
#include "Welcome.h"
#include "Editor.h"

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    if (argc == 1) {
        Welcome* rs = welcome_new();
        welcome_set_quit_on_destroy(rs, true);
        if (welcome_run(rs))
            gtk_main();
    } else if (argv[1][0] == '-') {
        if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
            printf("WeźMnie - system rezerwacji sprzętu\n");
            printf("Użycie: %s [-v|--version|-h|--help|FILE]\n", argv[0]);
        } else if (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0) {
            printf("1.0.0\n");
        } else {
            printf("Nieznana opcja: %s\n", argv[1]);
            printf("Wpisz --help, aby uzyskać pomoc.\n");
            return 1;
        }
    } else {
        FILE *f = fopen(argv[1], "rb");
        bool overwrite = f == NULL;
        if (f != NULL) fclose(f);
        Editor* re = editor_new(argv[1], overwrite);
        editor_set_quit_on_destroy(re, true);
        if (editor_run(re))
            gtk_main();
    }

    return 0;
}
