/*******************************************************************************
 * File: sensor_task.c
 *
 * Description:
 *   Simulates a temperature sensor at HIGH priority. Every 200 ticks,
 *   increments a fake reading and writes it to the IPC queue.
 *
 *   Demonstrates: high-priority scheduling, IPC write, rtos_sleep_ticks.
 ******************************************************************************/

#include "examples.h"
#include "../gazelle.h"
#include "../tinylibc/tinylibc.h"

void sensor_task(void)
{
    static uint16_t reading = 0;
    char msg[IPC_DATA_SIZE];

    reading++;

    /* Compose "reading:N" directly into msg — avoids strcat. */
    tinylibc_strncpy(msg, "reading:", IPC_DATA_SIZE);
    tinylibc_itoa(reading, msg + 8, 10);

    if (!ipc_write(msg, (uint8_t)(tinylibc_strlen(msg) + 1)))
        tinylibc_printf("[sensor] queue full, dropped reading %u\n", reading);

    rtos_sleep_ticks(200);
}
