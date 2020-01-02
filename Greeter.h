#pragma once
#include <stdbool.h>

typedef struct Greeter Greeter;
Greeter* greeter_new();
void greeter_set_quit_on_destroy(Greeter *this, bool quit_on_destroy);
bool greeter_start(Greeter *this);
