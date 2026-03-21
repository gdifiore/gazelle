/*******************************************************************************
 * File: idle_task.c
 *
 * Description:
 *   Idle task for the gazelle RTOS. Runs when no other tasks are ready.
 *   Yields immediately to keep the scheduler responsive.
 *
 * Author: Gabe DiFiore
 * Created: 2025-05-18
 *
 * Target MCU: LM3S6965EVB (ARM Cortex-M3)
 * Toolchain: arm-none-eabi-gcc
 * Platform: QEMU lm3s6965evb
 *
 * Notes:
 *   - TODO: replace busy-yield with a WFI (Wait For Interrupt) instruction
 *     once the preemptive scheduler lands, to reduce power consumption.
 ******************************************************************************/

#include "rtos.h"

void idle_task(void)
{
    rtos_sleep_ticks(1);
}
