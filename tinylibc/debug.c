#include "debug.h"
#include "io.h"

void tinylibc_panic(const char *msg) {
    tinylibc_puts("PANIC: ");
    tinylibc_puts(msg);
    tinylibc_puts("\n");
    while (1); // halt
}

void tinylibc_assert(bool cond, const char *msg) {
    if (!cond) tinylibc_panic(msg);
}
