/*******************************************************************************
 * File: idle_task.c
 *
 * Description:
 *   Idle task for the gazelle RTOS. Runs when no other tasks are ready.
 *   Currently yields control immediately to keep the scheduler responsive.
 *
 * Author: Gabe DiFiore
 * Created: 2025-05-18
 *
 * Target MCU: ATmega328P
 * Toolchain: avr-gcc, avr-libc
 * Platform: Arduino Nano (bare-metal)
 ******************************************************************************/

#include "../tasks.h"
#include "../rtos.h"
#include "../tinylibc/io.h"

void idle_task(void)
{
    // TODO: Enter into sleep mode

    // Minimal idle task: yield immediately to keep scheduler responsive
    static uint8_t counter = 0;
    if (counter++ % 10 == 0)
    {
        tinylibc_printf("IDLE (Tick: %u)\n", system_ticks);
    }

    rtos_sleep_ticks(1);
}