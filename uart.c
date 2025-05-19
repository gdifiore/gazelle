/*******************************************************************************
 * File: uart.c
 *
 * Description:
 *   UART driver for ATmega328P.
 *   Provides initialization, blocking transmit, and optional
 *   interrupt-driven receive support using an external ring buffer.
 *
 * Author: Gabe DiFiore
 * Created: 2025-05-14
 * Updated: 2025-05-19
 *
 * Notes:
 *   - Uses interrupt-driven RX with ring buffer if enabled.
 *   - Polling used for transmit.
 ******************************************************************************/

#include <avr/io.h>
#include "uart.h"
#include <util/setbaud.h>
#include <avr/interrupt.h>
#include "uart_buffer.h"

void uart_init(void)
{
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

#if USE_2X
    UCSR0A |= (1 << U2X0);
#endif

    UCSR0B |= (1 << RXEN0) | (1 << TXEN0); // Enable RX and TX
    UCSR0B |= (1 << RXCIE0);               // Enable RX interrupt

    uart_buffer_init();
    sei(); // Enable global interrupts
}

void uart_transmit(uint8_t data)
{
    while (!(UCSR0A & (1 << UDRE0)))
        ; // Wait for empty transmit buffer
    UDR0 = data;
    while (!(UCSR0A & (1 << TXC0)))
        ;                  // Wait for complete transmit
    UCSR0A |= (1 << TXC0); // Clear TXC flag
}

uint8_t uart_receive(void)
{
    while (!(UCSR0A & (1 << RXC0)))
        ; // Wait for data
    return UDR0;
}

void uart_transmit_burst(const uint8_t *data, size_t len)
{
    while (len--)
        uart_transmit(*data++);
}

void uart_receive_burst(uint8_t *data, size_t len)
{
    while (len--)
        *data++ = uart_receive();
}

ISR(USART_RX_vect)
{
    uart_buffer_put(UDR0); // read byte and enqueue into ring buffer
}
