/*******************************************************************************
 * File: main.c
 *
 * Description:
 *   Entry point for the gazelle RTOS sensor pipeline demo.
 *
 *   sensor_task    [HIGH]   — generates a fake reading every 200 ticks and
 *                             writes it to the IPC queue
 *   display_task   [MEDIUM] — reads from the IPC queue and prints each
 *                             reading over UART
 *   heartbeat_task [LOW]    — prints a periodic status line every 500 ticks;
 *                             only runs when sensor and display are sleeping
 *
 *   uart_mutex serialises UART output between display_task and heartbeat_task.
 *
 * Author: Gabe DiFiore
 * Created: 2025-05-08
 *
 * Target MCU: LM3S6965EVB (ARM Cortex-M3)
 * Toolchain: arm-none-eabi-gcc
 * Platform: QEMU lm3s6965evb
 ******************************************************************************/

#include "gazelle.h"
#include "examples/examples.h"

Semaphore uart_mutex;

int main(void)
{
    rtos_init();
    uart_init();
    ipc_init();

    rtos_sem_init(&uart_mutex, 1);

    rtos_create_task(sensor_task,    HIGH);
    rtos_create_task(display_task,   MEDIUM);
    rtos_create_task(heartbeat_task, LOW);

    rtos_start();

    return 0;
}
