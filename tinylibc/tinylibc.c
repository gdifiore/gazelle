/*******************************************************************************
 * File: tinylibc.c
 *
 * Description:
 *   Minimal implementation of standard C library functions for embedded systems.
 *   Provides basic I/O functionality through UART.
 *
 * Author: Gabe DiFiore
 * Created: 2025-05-08
 *
 * Notes:
 *   - Implements printf-like functionality with a reduced feature set
 *   - Uses UART for I/O operations
 *   - Limited buffer sizes to conserve memory
 ******************************************************************************/

#include "tinylibc.h"

void tinylibc_init(void) {
    uart_init();  // Sets up stdout and stdin
}

int tinylibc_putchar(char c, FILE *stream) {
    uart_transmit((uint8_t)c);
    return 0;
}

int tinylibc_printf(const char *fmt, ...) {
    char buffer[64];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    char *p = buffer;
    while (*p) {
        tinylibc_putchar(*p++, stdout);
    }
    return 0;
}
