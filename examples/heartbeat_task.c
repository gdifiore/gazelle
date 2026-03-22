/*******************************************************************************
 * File: heartbeat_task.c
 *
 * Description:
 *   Prints a periodic status line at LOW priority every 500 ticks. Competes
 *   with display_task for uart_mutex, demonstrating semaphore contention
 *   across priority levels.
 *
 *   Because this task is LOW priority, it only runs when both sensor_task
 *   and display_task are sleeping — the scheduler will never preempt either
 *   of them to run this task.
 *
 *   Demonstrates: low-priority scheduling, semaphore contention.
 ******************************************************************************/

#include "examples.h"
#include "../gazelle.h"
#include "../tinylibc/tinylibc.h"

void heartbeat_task(void)
{
    if (rtos_sem_wait(&uart_mutex) != ERR_OK)
        return;

    tinylibc_printf("[heartbeat] tick: %u\n", system_ticks);

    rtos_sem_signal(&uart_mutex);
    rtos_sleep_ticks(500);
}
