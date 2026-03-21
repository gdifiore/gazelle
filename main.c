/*******************************************************************************
 * File: main.c
 *
 * Description:
 *   Main file for starting RTOS execution. Initializes the system,
 *   creates tasks, and starts the RTOS scheduler.
 *
 * Author: Gabe DiFiore
 * Created: 2025-05-08
 *
 * Target MCU: LM3S6965EVB (ARM Cortex-M3)
 * Toolchain: arm-none-eabi-gcc
 * Platform: QEMU lm3s6965evb
 ******************************************************************************/

#include "kernel/rtos.h"
#include "tasks/tasks.h"
#include "drivers/uart.h"
#include "drivers/ipc.h"
#include "tinylibc/tinylibc.h"

int main(void)
{
    rtos_init();

    uart_init(); // Initialize UART and enable global interrupts
    ipc_init();  // Initialize IPC via shared memory

    rtos_start();

    return 0;
}