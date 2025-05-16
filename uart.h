#ifndef UART_H_
#define UART_H_

#include <stddef.h>  // size_t type
#include <stdint.h>  // uint8_t type
#include <stdio.h>   // FILE type

// Default baud rate set to 9600 if not defined
#ifndef BAUD
    #define BAUD 9600
#endif

/** UART output stream for stdout. */
extern FILE uart_out;
/** UART input stream for stdin. */
extern FILE uart_in;

/**
 * Initializes UART peripheral for transmitting and receiving.
 * Configures stdout and stdin to use UART unless NO_UART_STD is defined.
 * Baud rate is configured using setbaud.h.
 */
void uart_init(void);

/**
 * Transmits a single byte, blocking until transmission is complete.
 *
 * @param data Byte to transmit
 */
void uart_transmit(uint8_t data);

/**
 * Transmits multiple bytes from the specified buffer, blocking until complete.
 *
 * @param data Pointer to the bytes to transmit
 * @param len Number of bytes to transmit
 */
void uart_transmit_burst(uint8_t *data, size_t len);

/**
 * Receives a single byte, blocking until data is available.
 *
 * @return The received byte
 */
uint8_t uart_receive(void);

/**
 * Receives multiple bytes into the specified buffer, blocking until complete.
 *
 * @param data Pointer to store received bytes
 * @param len Number of bytes to receive
 */
void uart_receive_burst(uint8_t *data, size_t len);

#endif // UART_H_