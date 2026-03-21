/*******************************************************************************
 * File: task1.c
 *
 * Description:
 *   Implementation of task1 for the RTOS. Outputs a message and then delays.
 *
 * Author: Gabe DiFiore
 * Created: 2025-05-08
 *
 * Target MCU: LM3S6965EVB (ARM Cortex-M3)
 * Toolchain: arm-none-eabi-gcc
 * Platform: QEMU lm3s6965evb
 *
 * Notes:
 *   Simple demo task: prints "Task 1" and sleeps for 100 ticks.
 ******************************************************************************/

#include "tasks.h"
#include "../tinylibc/io.h"
#include "../kernel/rtos.h"

void task1(void)
{
    tinylibc_printf("Task 1\n");

    rtos_sleep_ticks(100);
}