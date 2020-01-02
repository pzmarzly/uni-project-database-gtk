#pragma once
#include <stdbool.h>

typedef struct Welcome Welcome;
Welcome* welcome_new();
void welcome_set_quit_on_destroy(Welcome *this, bool quit_on_destroy);
bool welcome_start(Welcome *this);
