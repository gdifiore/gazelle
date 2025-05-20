#ifndef RTOS_H
#define RTOS_H

#include "types.h"
#include <avr/io.h>

#define MAX_TASKS 5

typedef enum
{
    TASK_READY,
    TASK_SLEEPING,
    TASK_BLOCKED
} TaskState;

// Task priorities: lower value = higher priority
typedef enum
{
    HIGH = 0,
    MEDIUM = 1,
    LOW = 2,
    IDLE = 3
} TaskPriority;

extern volatile uint32_t system_ticks;

void rtos_init(void);
void rtos_create_task(void (*task_fn)(void), TaskPriority task_priority);
void rtos_start(void);
void rtos_delay(uint16_t ms);
void rtos_sleep_ticks(uint32_t ticks);
void rtos_sem_init(uint8_t sem_id, uint8_t initial_value);

#endif