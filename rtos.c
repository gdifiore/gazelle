/*******************************************************************************
 * File: rtos.c
 *
 * Description:
 *   Minimal cooperative RTOS kernel with static task scheduler.
 *   Provides basic task creation, scheduling, and delay functionality.
 *
 * Author: Gabe DiFiore
 * Created: 2025-05-08
 *
 * Notes:
 *   - Uses cooperative multitasking (non-preemptive)
 *   - Currently supports up to 2 tasks
 *   - No priority scheduling implemented
 ******************************************************************************/

#include "rtos.h"
#include "timer.h"
#include <util/delay.h>

volatile uint32_t system_ticks = 0;

typedef enum {
    TASK_READY,
    TASK_SLEEPING
} TaskState;

typedef struct {
    void (*fn)(void);
    uint32_t wake_tick;
    TaskState state;
} Task;

#define MAX_TASKS 2
static Task tasks[MAX_TASKS];
static int task_count = 0;
static int current_task_index = 0;

void rtos_init(void) {
    task_count = 0;
    for (int i = 0; i < MAX_TASKS; i++) {
        tasks[i].state = TASK_READY;
        tasks[i].wake_tick = 0;
    }
}

void rtos_create_task(void (*task_fn)(void)) {
    if (task_count < MAX_TASKS) {
        tasks[task_count].fn = task_fn;
        tasks[task_count].wake_tick = 0;
        tasks[task_count].state = TASK_READY;
        task_count++;
    }
}

// blocking
void rtos_delay(uint16_t ms) {
    while (ms--) _delay_ms(1);
}

static void tick_isr_callback(void) {
    system_ticks++;
}

void rtos_start(void) {
    timer_init(1000); // 1ms tick
    timer_set_callback(tick_isr_callback);
    timer_start();

    while (1) {
        for (int i = 0; i < task_count; ++i) {
            // Check if task is ready to run
            if (tasks[i].state == TASK_READY ||
                (tasks[i].state == TASK_SLEEPING && system_ticks >= tasks[i].wake_tick)) {

                // Wake up the task if it was sleeping
                if (tasks[i].state == TASK_SLEEPING) {
                    tasks[i].state = TASK_READY;
                }

                current_task_index = i;
                tasks[i].fn(); // cooperative task call
            }
        }
    }
}

// non_blocking
void rtos_sleep_ticks(uint32_t ticks) {
    tasks[current_task_index].wake_tick = system_ticks + ticks;
    tasks[current_task_index].state = TASK_SLEEPING;
}