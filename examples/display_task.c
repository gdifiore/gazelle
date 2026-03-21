/*******************************************************************************
 * File: display_task.c
 *
 * Description:
 *   Reads sensor readings from the IPC queue at MEDIUM priority and prints
 *   them over UART. Acquires uart_mutex before printing to prevent output
 *   interleaving with heartbeat_task.
 *
 *   Demonstrates: IPC read, semaphore as mutex, priority below sensor_task.
 *
 * Notes:
 *   uart_mutex is acquired before reading IPC so that a message is never
 *   consumed from the queue without being printed.
 ******************************************************************************/

#include "examples.h"
#include "../gazelle.h"
#include "../tinylibc/tinylibc.h"

void display_task(void)
{
    /* Acquire UART access before reading IPC — never consume a message
       we can't immediately print. */
    if (!rtos_sem_wait(&uart_mutex))
        return;

    char msg[IPC_DATA_SIZE];
    uint8_t len;
    if (ipc_read(msg, sizeof(msg), &len))
        tinylibc_printf("[display] %s\n", msg);

    rtos_sem_signal(&uart_mutex);
    rtos_sleep_ticks(50);
}
