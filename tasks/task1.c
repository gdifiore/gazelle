/*******************************************************************************
 * File: task1.c
 *
 * Description:
 *   Implementation of task1 for the RTOS. Outputs a message and then delays.
 *
 * Author: Gabe DiFiore
 * Created: 2025-05-08
 *
 * Target MCU: ATmega328P
 * Toolchain: avr-gcc, avr-libc
 * Platform: Arduino Nano (bare-metal)
 *
 * Notes:
 *   This is a simple demo task that prints "Task 1" and then
 *   delays for 100ms before returning to the scheduler.
 *   Uses a semaphore to ensure exclusive UART access.
 ******************************************************************************/

#include "../tasks.h"
#include "../tinylibc/io.h"
#include "../rtos.h"

void task1(void)
{
    tinylibc_printf("Task 1\n");

    rtos_sleep_ticks(100);
}