/*******************************************************************************
 * File: task2.c
 *
 * Description:
 *   Implementation of task2 for the RTOS. Outputs a message and then delays.
 *
 * Author: Gabe DiFiore
 * Created: 2025-05-08
 *
 * Target MCU: ATmega328P
 * Toolchain: avr-gcc, avr-libc
 * Platform: Arduino Nano (bare-metal)
 *
 * Notes:
 *   This is a simple demo task that prints "Task 2" and then
 *   delays for 100ms before returning to the scheduler.
 *   Uses a semaphore to ensure exclusive UART access.
 ******************************************************************************/

#include "../tasks.h"
#include "../tinylibc/io.h"
#include "../rtos.h"

void task2(void)
{
    rtos_sem_acquire(SEM_UART);
    tinylibc_printf("Task 2\n");
    rtos_sem_release(SEM_UART);
    rtos_sleep_ticks(100);
}