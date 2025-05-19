#include "conv.h"
#include "string.h"

char *tinylibc_itoa(int val, char *buf, int base) {
    static const char digits[] = "0123456789ABCDEF";
    char *ptr = buf;
    unsigned int u = val;
    if (val < 0 && base == 10) {
        *ptr++ = '-';
        u = -val;
    }

    char tmp[16];
    int i = 0;
    do {
        tmp[i++] = digits[u % base];
        u /= base;
    } while (u);

    while (i--) *ptr++ = tmp[i];
    *ptr = '\0';
    return buf;
}

int tinylibc_atoi(const char *str) {
    int res = 0;
    int sign = 1;
    if (*str == '-') {
        sign = -1;
        str++;
    }
    while (*str >= '0' && *str <= '9') {
        res = res * 10 + (*str++ - '0');
    }
    return res * sign;
}
