/*
 * test_uart_buffer.c — host-native unit tests for the UART ring buffer.
 *
 * The ring buffer uses the "one slot empty" full/empty sentinel, so it holds
 * UART_BUFFER_SIZE - 1 bytes at most. Tests encode this assumption explicitly.
 */

#include <stdint.h>
#include <stdio.h>

#include "test_runner.h"
#include "../drivers/uart_buffer.h"

/* --- tests ---------------------------------------------------------------- */

static void test_empty_after_init(void)
{
    uart_buffer_init();
    ASSERT(!uart_available());
}

static void test_put_read_roundtrip(void)
{
    uart_buffer_init();
    uart_buffer_put(0xAB);
    ASSERT(uart_available());
    ASSERT(uart_read() == 0xAB);
    ASSERT(!uart_available());
}

static void test_fifo_ordering(void)
{
    uart_buffer_init();
    uart_buffer_put(1);
    uart_buffer_put(2);
    uart_buffer_put(3);
    ASSERT(uart_read() == 1);
    ASSERT(uart_read() == 2);
    ASSERT(uart_read() == 3);
    ASSERT(!uart_available());
}

static void test_read_empty_returns_zero(void)
{
    uart_buffer_init();
    ASSERT(uart_read() == 0);
}

static void test_capacity(void)
{
    /* Buffer holds exactly UART_BUFFER_SIZE - 1 bytes due to sentinel. */
    uart_buffer_init();
    for (int i = 0; i < UART_BUFFER_SIZE - 1; i++)
        uart_buffer_put((uint8_t)i);

    /* One more write should be silently dropped. */
    uart_buffer_put(0xFF);

    int count = 0;
    while (uart_available()) {
        uart_read();
        count++;
    }
    ASSERT(count == UART_BUFFER_SIZE - 1);
}

static void test_overflow_flag_set_on_drop(void)
{
    uart_buffer_init();
    for (int i = 0; i < UART_BUFFER_SIZE - 1; i++)
        uart_buffer_put((uint8_t)i);

    /* Buffer is full but overflow has not been triggered yet. */
    ASSERT(!uart_buffer_overflow());

    uart_buffer_put(0xFF); /* dropped — sets overflow */
    ASSERT(uart_buffer_overflow());
}

static void test_overflow_flag_clears_after_read(void)
{
    uart_buffer_init();
    /* Overflow at the (UART_BUFFER_SIZE)th write. */
    for (int i = 0; i < UART_BUFFER_SIZE; i++)
        uart_buffer_put((uint8_t)i);

    ASSERT(uart_buffer_overflow());  /* first read: true, clears flag */
    ASSERT(!uart_buffer_overflow()); /* second read: false */
}

static void test_wraparound(void)
{
    /* Advance head and tail to the midpoint, then fill to force index wrap. */
    uart_buffer_init();

    for (int i = 0; i < UART_BUFFER_SIZE / 2; i++)
        uart_buffer_put((uint8_t)i);
    for (int i = 0; i < UART_BUFFER_SIZE / 2; i++)
        uart_read();

    /* Head and tail are now at index UART_BUFFER_SIZE/2.  Write enough to
       wrap both around the end of the array. */
    for (int i = 0; i < UART_BUFFER_SIZE - 1; i++)
        uart_buffer_put((uint8_t)(i + 0x80));

    for (int i = 0; i < UART_BUFFER_SIZE - 1; i++)
        ASSERT(uart_read() == (uint8_t)(i + 0x80));

    ASSERT(!uart_available());
}

/* --- entry point ---------------------------------------------------------- */

int main(void)
{
    printf("uart_buffer\n");
    RUN(test_empty_after_init);
    RUN(test_put_read_roundtrip);
    RUN(test_fifo_ordering);
    RUN(test_read_empty_returns_zero);
    RUN(test_capacity);
    RUN(test_overflow_flag_set_on_drop);
    RUN(test_overflow_flag_clears_after_read);
    RUN(test_wraparound);
    REPORT("uart_buffer");
}
