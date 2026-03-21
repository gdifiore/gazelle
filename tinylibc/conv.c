/*******************************************************************************
 * File: conv.c
 *
 * Description:
 *   Minimal conversion functions for the tinylibc library. Provides integer-to-string
 *   and string-to-integer conversion for basic numeric operations.
 *
 * Author: Gabe DiFiore
 * Created: 2025-05-14
 *
 * Target MCU: LM3S6965EVB (ARM Cortex-M3)
 * Toolchain: arm-none-eabi-gcc
 * Platform: QEMU lm3s6965evb
 *
 * Notes:
 *   - Supports decimal and hexadecimal conversion for itoa.
 ******************************************************************************/

#include "conv.h"
#include "string.h"

char *tinylibc_itoa(int val, char *buf, int base)
{
    char *ptr = buf;
    unsigned int u = val;
    if (val < 0 && base == 10)
    {
        *ptr++ = '-';
        u = -val;
    }

    char tmp[33];
    _Static_assert(sizeof(tmp) >= sizeof(unsigned int) * 8 + 1,
                   "tmp too small for worst-case binary representation of unsigned int");
    int i = 0;
    do
    {
        tmp[i++] = digits[u % base];
        u /= base;
    } while (u);

    while (i--)
        *ptr++ = tmp[i];
    *ptr = '\0';
    return buf;
}

int tinylibc_atoi(const char *str)
{
    int res = 0;
    int sign = 1;
    if (*str == '-')
    {
        sign = -1;
        str++;
    }
    while (*str >= '0' && *str <= '9')
    {
        res = res * 10 + (*str++ - '0');
    }
    return res * sign;
}
