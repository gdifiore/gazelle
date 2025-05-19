#ifndef TINYLIBC_IO_H
#define TINYLIBC_IO_H

#include <stdint.h>
#include <stddef.h>

void tinylibc_putchar(char c);
void tinylibc_puts(const char *s);
void tinylibc_printf(const char *fmt, ...);

#endif
