/*******************************************************************************
 * File: uart.c
 *
 * Description:
 *   UART0 driver for the LM3S6965EVB (Cortex-M3).
 *   Provides initialisation, polling transmit, and interrupt-driven receive
 *   via an external ring buffer.
 *
 * Author: Gabe DiFiore
 * Created: 2025-05-14
 * Updated: 2025-05-19
 *
 * Notes:
 *   - Uses interrupt-driven RX with ring buffer.
 *   - Polling used for transmit.
 ******************************************************************************/

#include "uart.h"
#include "uart_buffer.h"
#include "../kernel/rtos.h"
#include "../bsp/hw.h"


void uart_init(void)
{
    /* Enable clocks for UART0 and GPIO Port A */
    SYSCTL_RCGC1 |= SYSCTL_RCGC1_UART0;
    SYSCTL_RCGC2 |= SYSCTL_RCGC2_GPIOA;

    /* Configure PA0 (RX) and PA1 (TX) for UART alternate function */
    GPIOA_AFSEL |= (1u << 0) | (1u << 1);
    GPIOA_DEN   |= (1u << 0) | (1u << 1);

    /* Disable UART0 before configuring */
    UART0_CTL = 0;

    /*
     * Baud rate divisor:
     *   IBRD = F_CPU / (16 * BAUD)
     *   FBRD = round(frac * 64)
     *        = ((F_CPU % (16*BAUD)) * 64 + (8*BAUD)) / (16*BAUD)
     */
#define _UART_DIV (16u * BAUD)
    UART0_IBRD = F_CPU / _UART_DIV;
    UART0_FBRD = ((F_CPU % _UART_DIV) * 64u + _UART_DIV / 2u) / _UART_DIV;
#undef _UART_DIV

    /* 8N1, FIFOs enabled — must be written after baud divisors */
    UART0_LCRH = UART_LCRH_WLEN_8 | UART_LCRH_FEN;

    /* Enable RX interrupt and unmask it in NVIC */
    UART0_IM = UART_IM_RXIM;
    NVIC_EN0 |= UART0_IRQ_BIT;

    /* Enable UART0 with TX and RX */
    UART0_CTL = UART_CTL_UARTEN | UART_CTL_TXE | UART_CTL_RXE;

    uart_buffer_init();
}

void uart_transmit(uint8_t data)
{
    while (UART0_FR & UART_FR_TXFF)
        ; /* Wait for space in TX FIFO */
    UART0_DR = data;
}

int16_t uart_receive(uint32_t timeout_ms)
{
    if (timeout_ms == 0) {
        while (UART0_FR & UART_FR_RXFE)
            ; /* Wait for data in RX FIFO */

        return (uint8_t)UART0_DR;
    }

    uint32_t start = system_ticks;
    while ( (UART0_FR & UART_FR_RXFE) && ( (system_ticks - start) < timeout_ms) )
        ; /* Wait for data in RX FIFO */

    if (UART0_FR & UART_FR_RXFE)
        return -1; // timed out, FIFO still empty
    return (uint8_t)UART0_DR;
}

void uart_transmit_burst(const uint8_t *data, size_t len)
{
    while (len--)
        uart_transmit(*data++);
}

void uart_receive_burst(uint8_t *data, size_t len)
{
    while (len--)
        *data++ = uart_receive(0);
}

void UART0_Handler(void)
{
    /* Drain the RX FIFO — reading stops the interrupt */
    while (!(UART0_FR & UART_FR_RXFE))
        uart_buffer_put((uint8_t)UART0_DR);
}
