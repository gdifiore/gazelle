/*******************************************************************************
 * File: timer.c
 *
 * Description:
 *   Timer1-based system tick driver for the ATmega328P.
 *   Initializes the timer in CTC mode and invokes a user-supplied
 *   callback function at a configurable interval.
 *
 * Author: Gabe DiFiore
 * Created: 2025-05-19
 ******************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer.h"

static void (*user_callback)(void) = 0;

void timer_init(uint16_t tick_hz) {
    TCCR1A = 0; // Normal mode
    TCCR1B = (1 << WGM12); // CTC mode
    OCR1A = (F_CPU / 1024 / tick_hz) - 1; // Match for desired tick rate
    TIMSK1 |= (1 << OCIE1A); // Enable Compare A interrupt
}

void timer_start(void) {
    TCNT1 = 0;
    TCCR1B |= (1 << CS12) | (1 << CS10); // Prescaler 1024
}

void timer_stop(void) {
    TCCR1B &= ~((1 << CS12) | (1 << CS10)); // Stop timer
}

void timer_set_callback(void (*callback)(void)) {
    user_callback = callback;
}

ISR(TIMER1_COMPA_vect) {
    if (user_callback) user_callback();
}
