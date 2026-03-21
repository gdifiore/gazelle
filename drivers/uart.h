#ifndef UART_H_
#define UART_H_

#include <stdint.h>
#include <stddef.h>

#ifndef BAUD
#define BAUD 115200
#endif

void uart_init(void);
void uart_transmit(uint8_t data);
int16_t uart_receive(uint32_t timeout_ms);
void uart_transmit_burst(const uint8_t *data, size_t len);
void uart_receive_burst(uint8_t *data, size_t len);

#endif // UART_H_
