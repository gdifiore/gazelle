/*******************************************************************************
 * File: uart_buffer.c
 *
 * Description:
 *   Ring buffer implementation for UART receive operations. Stores incoming UART
 *   data in a circular buffer for interrupt-driven reception.
 *
 * Author: Gabe DiFiore
 * Created: 2025-05-19
 *
 * Target MCU: ATmega328P
 * Toolchain: avr-gcc, avr-libc
 * Platform: Arduino Nano (bare-metal)
 *
 * Notes:
 *   - Used in conjunction with the UART driver for interrupt-driven RX.
 *   - Drops data on buffer overflow.
 ******************************************************************************/

#include "uart_buffer.h"

#define UART_BUFFER_SIZE 64
static volatile uint8_t buffer[UART_BUFFER_SIZE];
static volatile uint8_t head = 0;
static volatile uint8_t tail = 0;

void uart_buffer_init(void)
{
    head = tail = 0;
}

bool uart_available(void)
{
    return head != tail;
}

uint8_t uart_read(void)
{
    if (!uart_available())
        return 0;
    uint8_t val = buffer[tail];
    tail = (tail + 1) % UART_BUFFER_SIZE;
    return val;
}

void uart_buffer_put(uint8_t data)
{
    uint8_t next = (head + 1) % UART_BUFFER_SIZE;
    if (next != tail)
    {
        buffer[head] = data;
        head = next;
    }
    // else: overflow, drop byte
}
