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

typedef enum
{
    SEM_UART,
    SEM_IPC
} Semaphores;

// Task priorities: lower value = higher priority
typedef enum
{
    HIGH = 0,
    MEDIUM = 1,
    LOW = 2,
    IDLE = 3
} TaskPriority;

#define MAX_SEMAPHORES 2
#define MAX_WAITERS_PER_SEM MAX_TASKS

typedef struct {
    uint8_t available;  // 0 = taken, 1 = available
    uint8_t wait_queue[MAX_WAITERS_PER_SEM];
    uint8_t wait_count;
} Semaphore;

extern volatile uint32_t system_ticks;

void rtos_init(void);
void rtos_create_task(void (*task_fn)(void), TaskPriority task_priority);
void rtos_start(void);
void rtos_delay(uint16_t ms);
void rtos_sleep_ticks(uint32_t ticks);
void rtos_sem_init(uint8_t sem_id, uint8_t initial_value);
__bool rtos_sem_acquire(uint8_t sem_id);
void rtos_sem_release(uint8_t sem_id);

#endif