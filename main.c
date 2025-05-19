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
 * Target MCU: ATmega328P
 * Toolchain: avr-gcc, avr-libc
 * Platform: Arduino Nano (bare-metal)
 *
 * Notes:
 *   LED0 and LED1 are configured on PB0 and PB1 respectively.
 ******************************************************************************/

#include <avr/io.h>
#include <util/delay.h>
#include "rtos.h"
#include "tasks.h"
#include "uart.h"
#include "tinylibc/tinylibc.h"

int main(void)
{
    uart_init();
    rtos_init();

    rtos_create_task(task1);
    rtos_create_task(task2);

    rtos_start();

    while (1)
    {
    }
}