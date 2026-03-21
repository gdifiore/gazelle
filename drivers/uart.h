#ifndef UART_H_
#define UART_H_

#include <stdint.h>
#include <stddef.h>

#ifndef BAUD
#define BAUD 115200
#endif

/*
 * Initialise UART0 at BAUD baud (default 115200), 8N1, FIFOs enabled.
 *
 * Enables clocks for UART0 and GPIO Port A, configures PA0 (RX) and PA1 (TX)
 * for the UART alternate function, programs the baud-rate divisors, enables
 * the RX interrupt in the NVIC, and initialises the receive ring buffer.
 */
void uart_init(void);

/*
 * Transmit a single byte.
 *
 * Blocks until the TX FIFO has space, then writes data to UART0_DR (polling).
 */
void uart_transmit(uint8_t data);

/*
 * Receive a single byte from UART0.
 *
 * If timeout_ms is 0, blocks indefinitely until a byte arrives in the RX
 * FIFO.  If timeout_ms is non-zero, waits at most that many milliseconds
 * (using system_ticks) and returns -1 if the FIFO is still empty.
 *
 * On success, returns the received byte as a value in [0, 255].
 */
int16_t uart_receive(uint32_t timeout_ms);

/*
 * Transmit len bytes from data, calling uart_transmit() for each byte.
 */
void uart_transmit_burst(const uint8_t *data, size_t len);

/*
 * Receive len bytes into data, calling uart_receive(0) for each byte.
 *
 * Blocks without timeout; use only when data is expected to arrive.
 */
void uart_receive_burst(uint8_t *data, size_t len);

#endif // UART_H_
