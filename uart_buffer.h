#ifndef UART_BUFFER_H_
#define UART_BUFFER_H_

#include <stdint.h>
#include <stdbool.h>

void uart_buffer_init(void);
bool uart_available(void);
uint8_t uart_read(void);
void uart_buffer_put(uint8_t data); // called from ISR

#endif
