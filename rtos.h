#ifndef RTOS_H
#define RTOS_H

#include "types.h"
#include <avr/io.h>

extern volatile uint32_t system_ticks;

void rtos_init(void);
void rtos_create_task(void (*task_fn)(void));
void rtos_start(void);
void rtos_delay(uint16_t ms);
void rtos_sleep_ticks(uint32_t ticks);

#endif