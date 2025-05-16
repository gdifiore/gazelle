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
#include <util/delay.h>

#define MAX_TASKS 2

static void (*tasks[MAX_TASKS])(void);
static int task_count = 0;

void rtos_init(void) {
    task_count = 0;
}

void rtos_create_task(void (*task_fn)(void)) {
    if (task_count < MAX_TASKS) {
        tasks[task_count++] = task_fn;
    }
}

void rtos_delay(uint16_t ms) {
    while (ms--) _delay_ms(1);
}

void rtos_start(void) {
    while (1) {
        for (int i = 0; i < task_count; ++i) {
            tasks[i]();  // Cooperative — tasks must return
        }
    }
}
