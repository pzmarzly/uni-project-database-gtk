#include "Editor.h"
#include "Greeter.h"
#include "Utils.h"
#include <string.h>

// No CLI options provided.
static int nothing() {
  Greeter *greeter = greeter_new();
  greeter_set_quit_on_destroy(greeter, true);
  if (greeter_start(greeter))
    gtk_main();
  return 0;
}

// A flag was specified.
static int flags(char *argv[]) {
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
static int file(char *argv[]) {
  // Open the editor directly.
  RepoType repo_type = file_exists(argv[1]) ? RepoOpen : RepoNew;
  Editor *editor = editor_new(argv[1], repo_type);
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
