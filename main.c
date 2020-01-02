// Paweł Zmarzły 314569
// Projekt, grupa MPi śr 12-14

#include <stdlib.h>
#include <gtk/gtk.h>
#include "Welcome.h"
#include "Editor.h"

// No CLI options provided.
int nothing() {
    Welcome* welcome = welcome_new();
    welcome_set_quit_on_destroy(welcome, true);
    if (welcome_start(welcome))
        gtk_main();
    return 0;
}

// A flag was specified.
int flags(char *argv[]) {
    if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
        printf("WeźMnie - system rezerwacji sprzętu\n");
        printf("Użycie: %s [-v|--version|-h|--help|FILE]\n", argv[0]);
        return 0;
    }
    if (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0) {
        printf("1.0.0\n");
        return 0;
    }
    printf("Nieznana opcja: %s\n", argv[1]);
    printf("Wpisz --help, aby uzyskać pomoc.\n");
    return 1;
}

// Something else was provided - treat it as a file path.
int file(char *argv[]) {
    // Check if file exists.
    FILE *f = fopen(argv[1], "rb");
    bool overwrite = f == NULL;
    if (f != NULL) fclose(f);
    // Open the editor directly.
    Editor* editor = editor_new(argv[1], overwrite);
    editor_set_quit_on_destroy(editor, true);
    if (editor_start(editor))
        gtk_main();
    return 0;
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    if (argc == 1)
        return nothing();
    if (argv[1][0] == '-')
        return flags(argv);
    return file(argv);
}
