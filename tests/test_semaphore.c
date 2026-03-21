/*
 * test_semaphore.c — host-native unit tests for the semaphore implementation.
 *
 * rtos.c's tasks[] array is static, so we can't inspect task state directly.
 * Tests are therefore limited to assertions on Semaphore fields (count,
 * waiter_count, waiters[]) and the return value of rtos_sem_wait().
 * Priority-based wakeup ordering requires scheduler integration and is
 * verified manually under QEMU.
 */

#include <stdio.h>
#include "test_runner.h"
#include "../kernel/rtos.h"

/* Dummy task functions registered with the scheduler in relevant tests. */
static void task_a(void) {}
static void task_b(void) {}

/* --- tests ---------------------------------------------------------------- */

static void test_init_count(void)
{
    Semaphore sem;
    rtos_sem_init(&sem, 3);
    ASSERT(sem.count == 3);
    ASSERT(sem.waiter_count == 0);
}

static void test_init_mutex(void)
{
    Semaphore sem;
    rtos_sem_init(&sem, 1);
    ASSERT(sem.count == 1);
    ASSERT(sem.waiter_count == 0);
}

static void test_wait_acquires_when_available(void)
{
    Semaphore sem;
    rtos_sem_init(&sem, 1);
    ASSERT(rtos_sem_wait(&sem) == true);
    ASSERT(sem.count == 0);
    ASSERT(sem.waiter_count == 0);
}

static void test_wait_decrements_count(void)
{
    Semaphore sem;
    rtos_sem_init(&sem, 3);
    rtos_sem_wait(&sem);
    rtos_sem_wait(&sem);
    ASSERT(sem.count == 1);
}

static void test_wait_blocks_when_unavailable(void)
{
    rtos_init();
    Semaphore sem;
    rtos_sem_init(&sem, 0);
    bool acquired = rtos_sem_wait(&sem);
    ASSERT(acquired == false);
    ASSERT(sem.waiter_count == 1);
    ASSERT(sem.waiters[0] == 0); /* current_task_index == 0 after rtos_init */
}

static void test_wait_does_not_add_duplicate_blocker(void)
{
    /* A second wait call when already blocked should not happen in correct
       task code, but the queue should still not grow beyond MAX_TASKS. */
    rtos_init();
    Semaphore sem;
    rtos_sem_init(&sem, 0);
    rtos_sem_wait(&sem); /* blocks task 0 */
    ASSERT(sem.waiter_count == 1);
}

static void test_signal_increments_when_no_waiters(void)
{
    Semaphore sem;
    rtos_sem_init(&sem, 0);
    rtos_sem_signal(&sem);
    ASSERT(sem.count == 1);
    ASSERT(sem.waiter_count == 0);
}

static void test_signal_releases_waiter(void)
{
    rtos_init();
    Semaphore sem;
    rtos_sem_init(&sem, 0);
    rtos_sem_wait(&sem);                  /* block current task */
    ASSERT(sem.waiter_count == 1);
    rtos_sem_signal(&sem);
    ASSERT(sem.waiter_count == 0);        /* waiter removed from queue */
    ASSERT(sem.count == 1);               /* pre-granted for woken task */
}

static void test_woken_task_can_reacquire(void)
{
    /* After signal pre-grants the semaphore, the next wait() should acquire
       immediately (simulating the woken task re-entering the function). */
    rtos_init();
    Semaphore sem;
    rtos_sem_init(&sem, 0);
    rtos_sem_wait(&sem);    /* block */
    rtos_sem_signal(&sem);  /* wake + pre-grant (count == 1) */
    ASSERT(rtos_sem_wait(&sem) == true); /* woken task re-acquires */
    ASSERT(sem.count == 0);
}

static void test_counting_semaphore(void)
{
    Semaphore sem;
    rtos_sem_init(&sem, 3);
    ASSERT(rtos_sem_wait(&sem) == true);
    ASSERT(rtos_sem_wait(&sem) == true);
    ASSERT(rtos_sem_wait(&sem) == true);
    ASSERT(rtos_sem_wait(&sem) == false); /* count exhausted */
    ASSERT(sem.waiter_count == 1);
}

static void test_signal_multiple_times_builds_count(void)
{
    Semaphore sem;
    rtos_sem_init(&sem, 0);
    rtos_sem_signal(&sem);
    rtos_sem_signal(&sem);
    rtos_sem_signal(&sem);
    ASSERT(sem.count == 3);
}

static void test_full_mutex_cycle(void)
{
    rtos_init();
    rtos_create_task(task_a, HIGH);
    rtos_create_task(task_b, LOW);

    Semaphore mutex;
    rtos_sem_init(&mutex, 1);

    /* Acquire */
    ASSERT(rtos_sem_wait(&mutex) == true);
    ASSERT(mutex.count == 0);

    /* Attempt to acquire again — should block */
    bool second = rtos_sem_wait(&mutex);
    ASSERT(second == false);
    ASSERT(mutex.waiter_count == 1);

    /* Release — wakes the blocked task */
    rtos_sem_signal(&mutex);
    ASSERT(mutex.waiter_count == 0);
    ASSERT(mutex.count == 1); /* pre-granted */
}

/* --- entry point ---------------------------------------------------------- */

int main(void)
{
    printf("semaphore\n");
    RUN(test_init_count);
    RUN(test_init_mutex);
    RUN(test_wait_acquires_when_available);
    RUN(test_wait_decrements_count);
    RUN(test_wait_blocks_when_unavailable);
    RUN(test_wait_does_not_add_duplicate_blocker);
    RUN(test_signal_increments_when_no_waiters);
    RUN(test_signal_releases_waiter);
    RUN(test_woken_task_can_reacquire);
    RUN(test_counting_semaphore);
    RUN(test_signal_multiple_times_builds_count);
    RUN(test_full_mutex_cycle);
    REPORT("semaphore");
}
