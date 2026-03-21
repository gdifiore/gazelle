#ifndef RTOS_H
#define RTOS_H

#include "types.h"
#include <stdint.h>
#include <stdbool.h>

#define MAX_TASKS 5

typedef enum
{
    TASK_READY,
    TASK_SLEEPING,
    TASK_BLOCKED,
    TASK_ZOMBIE
} TaskState;

// Task priorities: lower value = higher priority
typedef enum
{
    HIGH = 0,
    MEDIUM = 1,
    LOW = 2,
    IDLE = 3
} TaskPriority;

typedef struct {
    uint8_t count;
    uint8_t waiters[MAX_TASKS];
    uint8_t waiter_count;
} Semaphore;

extern volatile uint32_t system_ticks;

/*
 * Initialise the kernel.
 *
 * Enables interrupts, registers the idle task at slot 0, and resets all
 * scheduler state.  Must be called before any other RTOS function.
 */
void rtos_init(void);

/*
 * Register a new task with the given priority.
 *
 * Returns true on success.  Returns false if the task table is full
 * (MAX_TASKS) or task_priority is out of range.
 */
bool rtos_create_task(void (*task_fn)(void), TaskPriority task_priority);

/*
 * Mark a task for deferred removal.
 *
 * Transitions the task to TASK_ZOMBIE; the scheduler compacts it out of the
 * task array at the top of the next scheduling cycle.  Safe to call from
 * within the running task itself (including self-removal).
 *
 * Returns false if task_fn is not found or refers to the idle task.
 */
bool rtos_remove_task(void (*task_fn)(void));

/*
 * Start the scheduler.  Never returns.
 *
 * Initialises SysTick at 1 kHz, then enters the priority-based round-robin
 * scheduling loop.
 */
void rtos_start(void);

/*
 * Busy-wait for ms milliseconds.
 *
 * Spins on system_ticks; does not yield to other tasks.  Prefer
 * rtos_sleep_ticks() in task code to allow other tasks to run while waiting.
 */
void rtos_delay(uint16_t ms);

/*
 * Yield the calling task for the given number of scheduler ticks.
 *
 * Transitions the current task to TASK_SLEEPING and records its wake tick.
 * The scheduler skips it until system_ticks reaches wake_tick, at which point
 * the task is transitioned back to TASK_READY.
 */
void rtos_sleep_ticks(uint32_t ticks);

/*
 * Initialise a semaphore with the given count.
 *
 * initial_count is the number of times rtos_sem_wait() can succeed without
 * a matching rtos_sem_signal().  Use 1 for a mutual-exclusion lock.
 */
void rtos_sem_init(Semaphore *sem, uint8_t initial_count);

/*
 * Acquire a semaphore.
 *
 * If count > 0, decrements it and returns true immediately.
 *
 * If count == 0, adds the calling task to the semaphore's wait queue,
 * transitions it to TASK_BLOCKED, and returns false.  The scheduler will not
 * run this task again until rtos_sem_signal() wakes it.
 *
 * Because the task must return before the scheduler can switch, treat a false
 * return as a cue to exit the task function immediately:
 *
 *   void my_task(void) {
 *       if (!rtos_sem_wait(&lock)) return;
 *       // critical section
 *       rtos_sem_signal(&lock);
 *   }
 */
bool rtos_sem_wait(Semaphore *sem);

/*
 * Release a semaphore.
 *
 * If tasks are blocked on this semaphore, the highest-priority waiter is
 * transitioned to TASK_READY and pre-granted the semaphore (count is
 * incremented so the woken task can acquire on its next invocation).
 *
 * If no tasks are waiting, increments count.
 */
void rtos_sem_signal(Semaphore *sem);

#endif
