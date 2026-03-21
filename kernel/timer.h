#ifndef TIMER_H_
#define TIMER_H_

#include <stdint.h>

/*
 * Configure SysTick for the given tick frequency.
 *
 * Sets the SysTick reload register so the counter fires tick_hz interrupts
 * per second.  Does not start the timer; call timer_start() after registering
 * a callback with timer_set_callback().
 */
void timer_init(uint16_t tick_hz);

/*
 * Register a callback to be invoked from the SysTick ISR on each tick.
 *
 * Replaces any previously registered callback.  Pass NULL to disable.
 */
void timer_set_callback(void (*callback)(void));

/*
 * Start SysTick.
 *
 * Enables the counter with the processor clock and the tick interrupt.
 */
void timer_start(void);

/*
 * Stop SysTick by clearing the control register.
 */
void timer_stop(void);

#endif
