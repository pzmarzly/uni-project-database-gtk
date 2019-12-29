#include "Utils.h"

#define _POSIX_C_SOURCE 200112L
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

char* basedir() {
    char* buf = malloc(8192);
    int len = readlink("/proc/self/exe", buf, 8192 - 1);
    buf[len] = '\0';
    *(strrchr(buf, '/')) = '\0';
    return buf;
}
