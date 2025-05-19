#include "string.h"

size_t tinylibc_strlen(const char *s) {
    size_t len = 0;
    while (s[len]) len++;
    return len;
}

int tinylibc_strcmp(const char *a, const char *b) {
    while (*a && (*a == *b)) {
        a++; b++;
    }
    return *(unsigned char *)a - *(unsigned char *)b;
}

char *tinylibc_strcpy(char *dest, const char *src) {
    char *ret = dest;
    while ((*dest++ = *src++));
    return ret;
}

void *tinylibc_memset(void *dest, int val, size_t n) {
    unsigned char *p = dest;
    while (n--) *p++ = (unsigned char)val;
    return dest;
}
