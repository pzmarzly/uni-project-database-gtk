// Paweł Zmarzły 314569
// Projekt, grupa MPi śr 12-14

#include <gtk/gtk.h>
#include "RepoSelect.h"

int main(int argc, char *argv[]) {
    gtk_init(argc, argv);

    RepoSelect rs = repo_select_new();
    repo_select_set_quit_on_destroy(true);
    repo_select_run(&rs);

    gtk_main();
    return 0;
}
