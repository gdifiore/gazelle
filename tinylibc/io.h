#ifndef TINYLIBC_IO_H
#define TINYLIBC_IO_H

#include <stdint.h>
#include <stddef.h>

typedef char* va_list;

#define va_start(ap, last) (ap = (char*)&last + sizeof(last))
#define va_arg(ap, type) (*(type*)((ap += sizeof(type)) - sizeof(type)))
#define va_end(ap) (ap = (char*)0)

void tinylibc_putchar(char c);
void tinylibc_puts(const char *s);
void tinylibc_printf(const char *fmt, ...);

#endif
