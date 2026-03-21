/*******************************************************************************
 * File: rtos.c
 *
 * Description:
 *   Minimal cooperative RTOS kernel with static task scheduler.
 *   Provides basic task creation, scheduling, and delay
 *
 * Author: Gabe DiFiore
 * Created: 2025-05-08
 *
 * Notes:
 *   - Uses cooperative multitasking (non-preemptive)
 *   - Supports up to MAX_TASKS tasks (including idle task)
 *   - Priority-based round-robin scheduling
 ******************************************************************************/

#include "rtos.h"
#include "timer.h"
#include "../tinylibc/tinylibc.h"

extern void idle_task(void);

volatile uint32_t system_ticks = 0;

typedef struct
{
    void (*fn)(void);
    TaskPriority priority;
    uint32_t wake_tick;
    TaskState state;
    bool is_idle;
} Task;

static Task tasks[MAX_TASKS];
static int task_count = 0;
static int current_task_index = 0;

static int rr_next_idx_at_priority[IDLE + 1]; // Size based on the number of priority levels

void rtos_init(void)
{
#ifdef __arm__
    __asm volatile("cpsie i" ::: "memory"); /* Enable interrupts */
#endif
    task_count = 0;
    for (int i = 0; i < MAX_TASKS; i++)
    {
        tasks[i].state = TASK_READY;
        tasks[i].wake_tick = 0;
        tasks[i].is_idle = false;
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

bool rtos_create_task(void (*task_fn)(void), TaskPriority task_priority)
{
    if (!task_fn || task_count >= MAX_TASKS || task_priority > IDLE)
    {
        return false;
    }

    tasks[task_count].fn = task_fn;
    tasks[task_count].priority = task_priority;
    tasks[task_count].wake_tick = 0;
    tasks[task_count].state = TASK_READY;
    tasks[task_count].is_idle = false;
    task_count++;

    return true;
}

bool rtos_remove_task(void (*task_fn)(void))
{
    int found_idx = -1;

    // Search for the task
    for (int i = 0; i < task_count; i++)
    {
        if (tasks[i].fn == task_fn)
        {
            // Prevent removal of idle task
            if (tasks[i].is_idle)
            {
                tinylibc_printf("RTOS: Cannot remove idle task\n");
                return false;
            }
            found_idx = i;
            break;
        }
    }

    if (found_idx == -1)
    {
        tinylibc_printf("RTOS: Task not found for removal\n");
        return false;
    }

    // Mark for deletion
    tasks[found_idx].state = TASK_ZOMBIE;

    return true;
}

void rtos_sem_init(Semaphore *sem, uint8_t initial_count)
{
    sem->count = initial_count;
    sem->waiter_count = 0;
}

bool rtos_sem_wait(Semaphore *sem)
{
    if (sem->count > 0)
    {
        sem->count--;
        return true;
    }

    if (sem->waiter_count < MAX_TASKS)
    {
        sem->waiters[sem->waiter_count++] = (uint8_t)current_task_index;
        tasks[current_task_index].state = TASK_BLOCKED;
    }
    else
    {
        tinylibc_panic("Semaphore wait queue full");
    }

    return false;
}

void rtos_sem_signal(Semaphore *sem)
{
    if (sem->waiter_count == 0)
    {
        sem->count++;
        return;
    }

    /* Find highest-priority non-stale waiter. */
    int8_t best_pos = -1;
    for (uint8_t i = 0; i < sem->waiter_count; i++)
    {
        uint8_t idx = sem->waiters[i];
        if (tasks[idx].state != TASK_BLOCKED)
            continue;
        if (best_pos == -1 || tasks[idx].priority < tasks[sem->waiters[best_pos]].priority)
        {
            best_pos = (int8_t)i;
        }
    }

    if (best_pos == -1)
    {
        /* All queued waiters were stale -- treat as no waiters. */
        sem->waiter_count = 0;
        sem->count++;
        return;
    }

    uint8_t task_idx = sem->waiters[best_pos];

    /* Shift remaining waiters left to fill the gap. */
    for (uint8_t i = (uint8_t)best_pos; i < sem->waiter_count - 1; i++)
        sem->waiters[i] = sem->waiters[i + 1];
    sem->waiter_count--;

    /* Wake the task and pre-grant so rtos_sem_wait() succeeds next run. */
    tasks[task_idx].state = TASK_READY;
    sem->count++;
}

/*
 * rtos_task_cleanup - Remove all zombie tasks from the task array.
 *
 * Called at the top of each scheduling cycle, before task selection.
 * Compacts the task array in place and updates task_count,
 * rr_next_idx_at_priority, and current_task_index accordingly.
 *
 * Deferred removal (via TASK_ZOMBIE) ensures that a task removing itself
 * mid-run does not corrupt the scheduler's loop index.
 */
static void rtos_task_cleanup(void)
{
    for (int i = 0; i < task_count; i++)
    {
        if (tasks[i].state == TASK_ZOMBIE)
        {
            // Clear task fields
            tasks[i].fn = NULL;
            tasks[i].priority = IDLE;
            tasks[i].wake_tick = 0;
            tasks[i].state = TASK_READY;

            // Shift tasks to fill the gap
            for (int j = i; j < task_count - 1; j++)
            {
                tasks[j] = tasks[j + 1];
            }

            // Clear the last task slot
            Task *last_task = &tasks[task_count - 1];
            last_task->fn = NULL;
            last_task->priority = IDLE;
            last_task->wake_tick = 0;
            last_task->state = TASK_READY;
            last_task->is_idle = false;

            // Decrement task count
            task_count--;

            // Update round-robin indices
            for (int p = 0; p <= IDLE; p++)
            {
                if (rr_next_idx_at_priority[p] > i)
                {
                    rr_next_idx_at_priority[p]--;
                }
                else if (rr_next_idx_at_priority[p] >= task_count)
                {
                    rr_next_idx_at_priority[p] = 0;
                }
            }

            if (current_task_index > i)
            {
                current_task_index--;
            }

            // Fix: consecutive zombie tasks would be skipped
            i--;
        }
    }
}

void rtos_delay(uint16_t ms)
{
    uint32_t start = system_ticks;
    while ((system_ticks - start) < ms)
        ; /* Busy-wait; system_ticks increments at 1 kHz */
}

/*
 * tick_isr_callback - Increment system_ticks on each SysTick interrupt.
 *
 * Registered with timer_set_callback() during rtos_start().  The scheduler
 * uses system_ticks as its time source for rtos_delay() and sleep/wake.
 */
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
        // Init to invalid priority to find highest ready task
        TaskPriority highest_ready_priority = IDLE + 1;
        int selected_task_idx = -1;
        // Index of next search
        int search_start_idx = 0;

        // Remove zombie tasks
        rtos_task_cleanup();

        for (int i = 0; i < task_count; i++)
        {
            // Start at round-robin index for current priority, wrap around
            int idx = (rr_next_idx_at_priority[highest_ready_priority] + i) % task_count;
            Task *task = &tasks[idx];
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

            if (is_actually_ready && task->priority < highest_ready_priority)
            {
                highest_ready_priority = task->priority;
                selected_task_idx = idx;
                // Set next round-robin start index
                search_start_idx = (idx + 1) % task_count;
            }
        }

        if (selected_task_idx == -1)
        {
            selected_task_idx = 0; // Fallback to idle task
            tasks[0].state = TASK_READY;
            tasks[0].wake_tick = 0;
        }

        // Update round-robin index for the selected priority
        rr_next_idx_at_priority[highest_ready_priority] = search_start_idx;
        current_task_index = selected_task_idx;
        // Execute the selected task
        tasks[selected_task_idx].fn();
    }
}

void rtos_sleep_ticks(uint32_t ticks)
{
    tasks[current_task_index].wake_tick = system_ticks + ticks;
    tasks[current_task_index].state = TASK_SLEEPING;
}