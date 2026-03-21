/*
 * stubs.c — host-test stubs for hardware-coupled tinylibc functions.
 *
 * tinylibc_printf is called by ipc.c for diagnostic output. On the target it
 * eventually drives UART0; here we suppress it so test output stays clean.
 */

/* Suppress output — ipc.c's diagnostic prints are noise during tests. */
void tinylibc_printf(const char *fmt, ...) { (void)fmt; }
