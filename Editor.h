#include <stdbool.h>

typedef struct Editor Editor;
Editor* editor_new(char *path, bool create);
void editor_set_quit_on_destroy(Editor *re, bool quit_on_destroy);
void editor_run(Editor *re);
