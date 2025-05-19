#ifndef TINYLIBC_DEBUG_H
#define TINYLIBC_DEBUG_H

#include <stdbool.h>

void tinylibc_assert(bool condition, const char *msg);
void tinylibc_panic(const char *msg);

#endif
