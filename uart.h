#ifndef UART_H_
#define UART_H_

#include <stdint.h>
#include <stddef.h>

#ifndef BAUD
#define BAUD 9600
#endif

void uart_init(void);
void uart_transmit(uint8_t data);
uint8_t uart_receive(void);
void uart_transmit_burst(const uint8_t *data, size_t len);
void uart_receive_burst(uint8_t *data, size_t len);

#endif // UART_H_
