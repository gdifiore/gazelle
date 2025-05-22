#ifndef TINYLIBC_STRING_H
#define TINYLIBC_STRING_H

#include <stddef.h>

size_t tinylibc_strlen(const char *s);
int tinylibc_strcmp(const char *a, const char *b);
char *tinylibc_strcpy(char *dest, const char *src);
char *tinylibc_strncpy(char *dest, const char *src, size_t n);
char *tinylibc_strcat(char *dest, const char *src);
void *tinylibc_memset(void *dest, int val, size_t n);

#endif // TINYLIBC_STRING_H