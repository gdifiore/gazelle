/*******************************************************************************
 * gazelle.h — public API umbrella header
 *
 * Include this single header to access the full gazelle RTOS API:
 *
 *   #include "gazelle.h"
 *
 *   int main(void) {
 *       rtos_init();
 *       uart_init();
 *       rtos_create_task(my_task, HIGH);
 *       rtos_start();
 *   }
 ******************************************************************************/

#ifndef GAZELLE_H
#define GAZELLE_H

#include "kernel/rtos.h"
#include "kernel/timer.h"
#include "drivers/uart.h"
#include "drivers/uart_buffer.h"
#include "drivers/ipc.h"

#endif /* GAZELLE_H */
