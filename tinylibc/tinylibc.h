#ifndef TINYLIBC_H
#define TINYLIBC_H

#include <avr/io.h>
#include <stdio.h>
#include <stdarg.h>
#include "../uart.h"

void tinylibc_init(void);

int tinylibc_putchar(char c, FILE *stream);

int tinylibc_printf(const char *fmt, ...);

#endif // TINYLIBC_H
