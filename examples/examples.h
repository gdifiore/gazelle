#ifndef EXAMPLES_H
#define EXAMPLES_H

#include "../kernel/rtos.h"

/*
 * Sensor pipeline demo tasks — registered in main.c.
 * See each task's .c file for a description of its role.
 */
void sensor_task(void);
void display_task(void);
void heartbeat_task(void);

/*
 * Shared UART mutex — defined in main.c, initialised before rtos_start().
 * Acquired by display_task and heartbeat_task before writing to UART.
 */
extern Semaphore uart_mutex;

#endif /* EXAMPLES_H */
