/*******************************************************************************
 * File: timer.c
 *
 * Description:
 *   SysTick-based system tick driver for the LM3S6965EVB (Cortex-M3).
 *   Initialises SysTick and invokes a user-supplied callback at a
 *   configurable tick rate.
 *
 * Author: Gabe DiFiore
 * Created: 2025-05-19
 ******************************************************************************/

#include "timer.h"
#include "../bsp/hw.h"

static void (*user_callback)(void) = 0;

void timer_init(uint16_t tick_hz)
{
    SYST_RVR = (F_CPU / tick_hz) - 1; /* Reload value for desired tick rate */
    SYST_CVR = 0;                      /* Clear current value                */
}

void timer_start(void)
{
    /* Enable SysTick with processor clock and exception request */
    SYST_CSR = SYST_CSR_ENABLE | SYST_CSR_TICKINT | SYST_CSR_CLKSOURCE;
}

void timer_stop(void)
{
    SYST_CSR = 0;
}

void timer_set_callback(void (*callback)(void))
{
    user_callback = callback;
}

void SysTick_Handler(void)
{
    if (user_callback)
        user_callback();
}
