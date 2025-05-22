/*******************************************************************************
 * File: string.c
 *
 * Description:
 *   String manipulation functions for the tinylibc library. Provides minimal
 *   implementations of strlen, strcmp, strcpy, strcat, and memset.
 *
 * Author: Gabe DiFiore
 * Created: 2025-05-14
 *
 * Target MCU: ATmega328P
 * Toolchain: avr-gcc, avr-libc
 * Platform: Arduino Nano (bare-metal)
 *
 * Notes:
 *   - Designed for minimal memory usage on resource-constrained devices.
 ******************************************************************************/

#include "string.h"

size_t tinylibc_strlen(const char *s)
{
    size_t len = 0;
    while (s[len])
        len++;
    return len;
}

int tinylibc_strcmp(const char *a, const char *b)
{
    while (*a && (*a == *b))
    {
        a++;
        b++;
    }
    return *(unsigned char *)a - *(unsigned char *)b;
}

char *tinylibc_strcpy(char *dest, const char *src)
{
    char *ret = dest;
    while ((*dest++ = *src++))
        ;
    return ret;
}

char *tinylibc_strncpy(char *dest, const char *src, size_t n)
{
    char *ret = dest;
    size_t i;
    for (i = 0; i < n && src[i]; i++) {
        dest[i] = src[i];
    }
    for (; i < n; i++) {
        dest[i] = '\0';
    }
    return ret;
}

char *tinylibc_strcat(char *dest, const char *src)
{
    char *ret = dest;
    while (*dest)
        dest++;
    while ((*dest++ = *src++))
        ;
    return ret;
}

void *tinylibc_memset(void *dest, int val, size_t n)
{
    unsigned char *p = dest;
    while (n--)
        *p++ = (unsigned char)val;
    return dest;
}
