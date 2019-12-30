#include <stdbool.h>

typedef struct RepoEditor RepoEditor;
RepoEditor* repo_editor_new(char *path, bool create);
void repo_editor_set_quit_on_destroy(RepoEditor *re, bool quit_on_destroy);
void repo_editor_run(RepoEditor *re);
