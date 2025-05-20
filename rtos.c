/*******************************************************************************
 * File: rtos.c
 *
 * Description:
 *   Minimal cooperative RTOS kernel with static task scheduler.
 *   Provides basic task creation, scheduling, delay, and semaphore functionality.
 *
 * Author: Gabe DiFiore
 * Created: 2025-05-08
 *
 * Notes:
 *   - Uses cooperative multitasking (non-preemptive)
 *   - Supports up to 3 tasks (including idle task)
 *   - No priority scheduling implemented
 *   - Semaphores for synchronization and mutual exclusion
 ******************************************************************************/

#include "rtos.h"
#include "timer.h"
#include "tasks.h"
#include "tinylibc/tinylibc.h"
#include <util/delay.h>

volatile uint32_t system_ticks = 0;

typedef struct
{
    void (*fn)(void);
    TaskPriority priority;
    uint32_t wake_tick;
    TaskState state;
    bool is_idle;
    uint8_t waiting_on_sem; // Semaphore ID the task is waiting on (0xFF if none)
} Task;

static Task tasks[MAX_TASKS];
static int task_count = 0;
static int current_task_index = 0;

static int rr_next_idx_at_priority[IDLE + 1]; // Size based on the number of priority levels

void rtos_init(void)
{
    task_count = 0;
    for (int i = 0; i < MAX_TASKS; i++)
    {
        tasks[i].state = TASK_READY;
        tasks[i].wake_tick = 0;
        tasks[i].is_idle = false;
        tasks[i].waiting_on_sem = 0xFF; // No semaphore
    }
    // Create idle task
    tasks[0].fn = idle_task;
    tasks[0].priority = IDLE;
    tasks[0].is_idle = true;
    tasks[0].state = TASK_READY;
    task_count = 1;

    // Initialize round-robin indices
    for (int i = 0; i <= IDLE; i++)
    {
        // Iterate through all valid priority values HIGH, MEDIUM, LOW, IDLE
        rr_next_idx_at_priority[i] = 0;
    }
}

void rtos_create_task(void (*task_fn)(void), TaskPriority task_priority)
{
    if (task_count >= MAX_TASKS)
    {
        tinylibc_panic("Max tasks exceeded");
    }
    if (task_priority > IDLE)
    {
        tinylibc_panic("Invalid task priority");
    }
    if (task_count < MAX_TASKS)
    {
        tasks[task_count].fn = task_fn;
        tasks[task_count].priority = task_priority;
        tasks[task_count].wake_tick = 0;
        tasks[task_count].state = TASK_READY;
        tasks[task_count].is_idle = false;
        tasks[task_count].waiting_on_sem = 0xFF;
        task_count++;
    }
}

void rtos_delay(uint16_t ms)
{
    while (ms--)
        _delay_ms(1);
}

static void tick_isr_callback(void)
{
    system_ticks++;
}

void rtos_start(void)
{
    timer_init(1000); // 1ms tick
    timer_set_callback(tick_isr_callback);
    timer_start();

    while (1)
    {
        TaskPriority highest_ready_priority = IDLE + 1;
        int selected_task_idx = -1;

        // Determine the highest priority level of any task that is ready to run
        for (int i = 0; i < task_count; i++)
        {
            Task *task = &tasks[i];

            bool is_actually_ready = false;
            if (task->state == TASK_READY)
            {
                is_actually_ready = true;
            }
            else if (task->state == TASK_SLEEPING && system_ticks >= task->wake_tick)
            {
                task->state = TASK_READY;
                task->wake_tick = 0;
                is_actually_ready = true;
            }

            if (is_actually_ready)
            {
                if (task->priority < highest_ready_priority)
                {
                    highest_ready_priority = task->priority;
                }
            }
        }

        // If highest_ready_priority is still IDLE + 1, no task is ready

        // Find the next task to run at 'highest_ready_priority' using round-robin
        if (highest_ready_priority <= IDLE)
        {
            // A valid priority level was found (HIGH, MEDIUM, LOW, or IDLE)
            int search_start_idx = rr_next_idx_at_priority[highest_ready_priority];

            // Iterate at most 'task_count' times to check all tasks, starting from 'search_start_idx'
            for (int i = 0; i < task_count; i++)
            {
                int current_check_idx = (search_start_idx + i) % task_count;
                Task *task = &tasks[current_check_idx];

                if (task->priority == highest_ready_priority)
                {
                    bool is_actually_ready = false;
                    if (task->state == TASK_READY)
                    {
                        is_actually_ready = true;
                    }
                    else if (task->state == TASK_SLEEPING && system_ticks >= task->wake_tick)
                    {
                        task->state = TASK_READY;
                        task->wake_tick = 0;
                        is_actually_ready = true;
                    }

                    if (is_actually_ready)
                    {
                        selected_task_idx = current_check_idx;
                        // Update the starting point for the next round-robin scan at this priority
                        rr_next_idx_at_priority[highest_ready_priority] = (current_check_idx + 1) % task_count;
                        break; // Found a task to run
                    }
                }
            }
        }

        // Fallback to Idle Task if no other task was selected
        // Happens if no are tasks ready or if all tasks at 'highest_ready_priority' were not actually runnable (e.g. all sleeping/waiting)
        if (selected_task_idx == -1)
        {
            for (int i = 0; i < task_count; i++)
            {
                if (tasks[i].is_idle)
                {
                    // Ensure idle task is runnable if it was sleeping
                    if (tasks[i].state == TASK_SLEEPING && system_ticks >= tasks[i].wake_tick)
                    {
                        tasks[i].state = TASK_READY;
                        tasks[i].wake_tick = 0;
                    }
                    // The idle task should generally always be in TASK_READY unless it explicitly sleeps
                    if (tasks[i].state == TASK_READY)
                    {
                        selected_task_idx = i;
                        break;
                    }
                }
            }
            // If still -1, and we must run something (assuming task[0] is always the idle_task and must be runnable)
            if (selected_task_idx == -1 && task_count > 0 && tasks[0].is_idle)
            {
                tasks[0].state = TASK_READY; // Force idle to be ready
                tasks[0].wake_tick = 0;
                selected_task_idx = 0;
            }
        }

        // Run the selected task
        if (selected_task_idx != -1)
        {
            current_task_index = selected_task_idx;
            Task *task_to_run = &tasks[current_task_index];

            // If the task was sleeping and is now waking up
            if (task_to_run->state == TASK_SLEEPING)
            {
                // Condition (system_ticks >= task_to_run->wake_tick) was met above
                task_to_run->state = TASK_READY;
                task_to_run->wake_tick = 0; // Clear wake tick

                // If it was waiting on a semaphore that is NOW available, it will proceed
            }

            // If the task was waiting for a semaphore that is now available, and it got selected,
            // its `waiting_on_sem` should be cleared when it successfully acquires it.
            // This is handled by `rtos_sem_acquire`: if successful, it sets state to READY and clears waiting_on_sem.
            // The scheduler just ensures it gets a chance to run if it's not blocked.

            task_to_run->fn();
        }
        else
        {
            // Should not be reached if an idle task exists and is always made schedulable
            // Critical error: no task could be scheduled
            // TODO: reset?
            tinylibc_panic("Critical Error: No task to schedule.");
            while (1)
                ; // Halt - system cannot proceed
        }
    }
}

void rtos_sleep_ticks(uint32_t ticks)
{
    tasks[current_task_index].wake_tick = system_ticks + ticks;
    tasks[current_task_index].state = TASK_SLEEPING;
}