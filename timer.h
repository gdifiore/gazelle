#ifndef TIMER_H_
#define TIMER_H_

#include <stdint.h>

void timer_init(uint16_t tick_hz); // e.g., 1000 for 1ms tick
void timer_set_callback(void (*callback)(void));
void timer_start(void);
void timer_stop(void);

#endif
