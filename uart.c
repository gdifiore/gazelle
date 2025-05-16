/*******************************************************************************
 * File: uart.c
 *
 * Description:
 *   UART driver for ATmega328P, providing initialization, data transmission,
 *   reception, and standard I/O stream integration.
 *
 * Author: Gabe DiFiore
 * Created: 2025-05-14
 *
 * Notes:
 *   - Baud rate is defined in uart.h
 *   - Implements blocking transmit and receive operations
 *   - Configures stdout/stdin by default (disable with NO_UART_STD)
 ******************************************************************************/

#include <avr/io.h>
#include "types.h"
#include "uart.h"
#include <util/setbaud.h>

/** Stream output function for stdout. */
static int uart_putchar(char c, FILE *stream)
{
    (void)stream; // Suppress unused parameter warning
    uart_transmit(c);
    return 0;
}

/** Stream input function for stdin. */
static int uart_getchar(FILE *stream)
{
    (void)stream; // Suppress unused parameter warning
    return uart_receive();
}

/** UART output stream setup. */
FILE uart_out = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
/** UART input stream setup. */
FILE uart_in = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);

void uart_init(void)
{
    // Configure baud rate using setbaud.h values
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

#if USE_2X
    UCSR0A |= (1 << U2X0); // Enable double speed mode
#endif

    // Enable transmitter and receiver
    UCSR0B |= (1 << RXEN0) | (1 << TXEN0);

#ifndef NO_UART_STD
    // Redirect standard I/O streams
    stdout = &uart_out;
    stdin = &uart_in;
#endif
}

/** Checks if a byte is ready to be read from the receive buffer. */
static bool uart_is_rx_ready(void)
{
    return UCSR0A & (1 << RXC0);
}

/** Checks if the transmit buffer is empty and ready for new data. */
static bool uart_is_tx_ready(void)
{
    return UCSR0A & (1 << UDRE0);
}

/** Checks if a transmission has fully completed. */
static bool uart_is_tx_complete(void)
{
    return UCSR0A & (1 << TXC0);
}

void uart_transmit(uint8_t data)
{
    // Wait for transmit buffer to be ready
    while (!uart_is_tx_ready())
        ;

    UDR0 = data;

    // Wait for transmission to complete
    while (!uart_is_tx_complete())
        ;
}

void uart_transmit_burst(uint8_t *data, size_t len)
{
    while (len--) {
        // Wait for transmit buffer to be ready
        while (!uart_is_tx_ready())
            ;

        UDR0 = *data++;
    }

    // Wait for final transmission to complete
    while (!uart_is_tx_complete())
        ;
}

uint8_t uart_receive(void)
{
    // Wait for data to be available
    while (!uart_is_rx_ready())
        ;

    return UDR0;
}

void uart_receive_burst(uint8_t *data, size_t len)
{
    while (len--) {
        *data++ = uart_receive();
    }
}