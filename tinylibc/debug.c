/*******************************************************************************
 * File: debug.c
 *
 * Description:
 *   Debugging utilities for the tinylibc library. Provides panic and assert
 *   functions for error handling and debugging.
 *
 * Author: Gabe DiFiore
 * Created: 2025-05-14
 *
 * Target MCU: LM3S6965EVB (ARM Cortex-M3)
 * Toolchain: arm-none-eabi-gcc
 * Platform: QEMU lm3s6965evb
 *
 * Notes:
 *   - Panic halts the system; assert checks conditions and triggers panic on failure.
 ******************************************************************************/

#include "debug.h"
#include "io.h"

void tinylibc_panic(const char *msg)
{
    tinylibc_puts("PANIC: ");
    tinylibc_puts(msg);
    tinylibc_puts("\n");
    while (1)
        ; // halt
}

void tinylibc_assert(bool cond, const char *msg)
{
    if (!cond)
        tinylibc_panic(msg);
}
