#ifndef TINYLIBC_CONV_H
#define TINYLIBC_CONV_H

static const char digits[] = "0123456789ABCDEF";

char *tinylibc_itoa(int val, char *buf, int base);
int tinylibc_atoi(const char *str);

#endif
