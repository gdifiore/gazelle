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
#include "tinylibc/tinylibc.h"

#ifdef DEBUG
#include "avr/avr_mcu_section.h"
AVR_MCU(F_CPU, "atmega328p");
AVR_MCU_VCD_FILE("main.vcd", 1000);

// Trace PB0 and PB1
const struct avr_mmcu_vcd_trace_t _vcd_trace[] __attribute__((section(".vcd_trace"))) = {
    { .tag = 0, .len = sizeof(struct avr_mmcu_vcd_trace_t) - 2, .mask = (1 << PB0), .what = &PORTB, .name = "LED0" },
    { .tag = 0, .len = sizeof(struct avr_mmcu_vcd_trace_t) - 2, .mask = (1 << PB1), .what = &PORTB, .name = "LED1" },
};
#endif

int main(void)
{
    DDRB |= (1 << PB0) | (1 << PB1); // Set PB0 and PB1 as output

    tinylibc_init();

    rtos_init();
    rtos_create_task(task1);
    rtos_create_task(task2);
    rtos_start();

    while (1)
    {
    }
}