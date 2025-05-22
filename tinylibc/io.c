/*******************************************************************************
 * File: io.c
 *
 * Description:
 *   Input/output functions for the tinylibc library. Provides UART-based
 *   character and string output, including a printf-like function.
 *
 * Author: Gabe DiFiore
 * Created: 2025-05-14
 *
 * Target MCU: ATmega328P
 * Toolchain: avr-gcc, avr-libc
 * Platform: Arduino Nano (bare-metal)
 *
 * Notes:
 *   - Relies on the UART driver for output operations.
 *   - Supports basic format specifiers (%d, %u, %x, %s, %%).
 ******************************************************************************/

#include "io.h"
#include "conv.h"
#include "../uart.h"

void tinylibc_putchar(char c)
{
    // Sending '\r' before '\n' for newlines caused simavr to append double periods ("..") in the UART output,
    // likely due to its handling of the \r\n sequence as two events. Commenting out the '\r' transmission
    // results in only '\n' being sent, reducing the simavr output artifact to a single period (".") per line.
    // if (c == '\n') uart_transmit('\r');
    uart_transmit((uint8_t)c);
}

void tinylibc_puts(const char *s)
{
    while (*s)
        tinylibc_putchar(*s++);
}

void tinylibc_printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    while (*fmt)
    {
        if (*fmt == '%' && *(fmt + 1))
        {
            fmt++;
            if (*fmt == 'd')
            {
                int n = va_arg(args, int);
                char buf[12];
                tinylibc_itoa(n, buf, 10);
                tinylibc_puts(buf);
            }
            else if (*fmt == 'u')
            {
                unsigned int n = va_arg(args, unsigned int);
                char buf[12];
                tinylibc_itoa(n, buf, 10);
                tinylibc_puts(buf);
            }
            else if (*fmt == 'x')
            {
                unsigned int n = va_arg(args, unsigned int);
                char buf[12];
                tinylibc_itoa(n, buf, 16);
                tinylibc_puts(buf);
            }
            else if (*fmt == 's')
            {
                char *s = va_arg(args, char *);
                tinylibc_puts(s);
            }
            else if (*fmt == '%')
            {
                tinylibc_putchar('%');
            }
        }
        else
        {
            tinylibc_putchar(*fmt);
        }
        fmt++;
    }
    va_end(args);
}
