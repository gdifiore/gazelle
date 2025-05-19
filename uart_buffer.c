#include "uart_buffer.h"

#define UART_BUFFER_SIZE 64
static volatile uint8_t buffer[UART_BUFFER_SIZE];
static volatile uint8_t head = 0;
static volatile uint8_t tail = 0;

void uart_buffer_init(void) {
    head = tail = 0;
}

bool uart_available(void) {
    return head != tail;
}

uint8_t uart_read(void) {
    if (!uart_available()) return 0;
    uint8_t val = buffer[tail];
    tail = (tail + 1) % UART_BUFFER_SIZE;
    return val;
}

void uart_buffer_put(uint8_t data) {
    uint8_t next = (head + 1) % UART_BUFFER_SIZE;
    if (next != tail) {
        buffer[head] = data;
        head = next;
    }
    // else: overflow, drop byte
}
