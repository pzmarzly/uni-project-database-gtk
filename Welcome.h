#include <stdbool.h>

typedef struct Welcome Welcome;
Welcome* welcome_new();
void welcome_set_quit_on_destroy(Welcome *rs, bool quit_on_destroy);
void welcome_run(Welcome *rs);
