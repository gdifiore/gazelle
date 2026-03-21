/*
 * stubs.c — host-test stubs for hardware-coupled functions.
 *
 * Suppresses output and no-ops hardware calls so driver and kernel code can
 * be compiled and exercised natively on a Linux host.
 */

#include <stdint.h>

/* tinylibc I/O ------------------------------------------------------------ */
void tinylibc_printf(const char *fmt, ...) { (void)fmt; }
void tinylibc_puts(const char *s)          { (void)s; }
void tinylibc_putchar(char c)              { (void)c; }
void tinylibc_panic(const char *msg)       { (void)msg; }
void tinylibc_assert(int cond, const char *msg) { (void)cond; (void)msg; }

/* Timer driver (only called from rtos_start(), not exercised in tests) ---- */
void timer_init(uint16_t hz)               { (void)hz; }
void timer_set_callback(void (*cb)(void))  { (void)cb; }
void timer_start(void)                     {}

/* Idle task (registered by rtos_init()) ----------------------------------- */
void idle_task(void) {}
