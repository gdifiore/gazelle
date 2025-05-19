#include "../tasks.h"
#include "../ipc.h"
#include "../rtos.h"
#include "../tinylibc/tinylibc.h"

// High Priority Test Tasks
void high_prio_task_A(void)
{
    for (int i = 0; i < 2; i++)
    { // Run a couple of times before sleeping to show RR
        tinylibc_printf("H_A (Tick: %u)\n", system_ticks);
        rtos_delay(10); // Simulate some work, very short delay not to miss RR with another fast H task
    }
    rtos_sleep_ticks(50); // Sleep for a noticeable duration
}

void high_prio_task_B(void)
{
    for (int i = 0; i < 2; i++)
    {
        tinylibc_printf("H_B (Tick: %u)\n", system_ticks);
        rtos_delay(10);
    }
    rtos_sleep_ticks(60);
}

// Medium Priority Test Tasks
void medium_prio_task_X(void)
{
    tinylibc_printf("M_X (Tick: %u)\n", system_ticks);
    rtos_sleep_ticks(30);
}

void medium_prio_task_Y(void)
{
    tinylibc_printf("M_Y (Tick: %u)\n", system_ticks);
    rtos_sleep_ticks(35);
}

void medium_prio_task_Z(void)
{
    tinylibc_printf("M_Z (Tick: %u)\n", system_ticks);
    rtos_sleep_ticks(40);
}

// Low Priority Test Task
void low_prio_task_1(void)
{
    tinylibc_printf("L_1 (Tick: %u)\n", system_ticks);
    rtos_sleep_ticks(100);
}

// Semaphore Test Tasks
void sem_holder_task(void)
{
    tinylibc_printf("SEM_HOLDER: Trying to acquire SEM_IPC (Tick: %u)\n", system_ticks);
    rtos_sem_acquire(SEM_IPC);
    tinylibc_printf("SEM_HOLDER: Acquired SEM_IPC! Holding... (Tick: %u)\n", system_ticks);
    rtos_sleep_ticks(70); // Hold semaphore for some time
    rtos_sem_release(SEM_IPC);
    tinylibc_printf("SEM_HOLDER: Released SEM_IPC (Tick: %u)\n", system_ticks);
    rtos_sleep_ticks(10); // Sleep before trying again or finishing
}

void sem_waiter_task(void)
{
    tinylibc_printf("SEM_WAITER: Trying to acquire SEM_IPC (Tick: %u)\n", system_ticks);
    rtos_sem_acquire(SEM_IPC);
    tinylibc_printf("SEM_WAITER: Acquired SEM_IPC! (Tick: %u)\n", system_ticks);
    rtos_sem_release(SEM_IPC);
    tinylibc_printf("SEM_WAITER: Released SEM_IPC (Tick: %u)\n", system_ticks);
    rtos_sleep_ticks(15);
}

// IPC Test Tasks
void ipc_producer_task(void)
{
    static uint32_t counter = 0;
    char data[IPC_DATA_SIZE];
    char num_buf[12];
    tinylibc_strcpy(data, "Data_");
    tinylibc_itoa(counter++, num_buf, 10);
    tinylibc_strcat(data, num_buf);

    rtos_sem_acquire(SEM_UART);
    tinylibc_printf("PRODUCER: Writing '%s' (Tick: %u)\n", data, system_ticks);
    rtos_sem_release(SEM_UART);

    if (ipc_write(data, tinylibc_strlen(data) + 1))
    {
        rtos_sem_acquire(SEM_UART);
        tinylibc_printf("PRODUCER: Write successful (Tick: %u)\n", system_ticks);
        rtos_sem_release(SEM_UART);
    }
    rtos_sleep_ticks(50); // Simulate work and allow consumer to run
}

void ipc_consumer_task(void)
{
    char data[IPC_DATA_SIZE];
    uint8_t len;
    if (ipc_read(data, IPC_DATA_SIZE, &len))
    {
        rtos_sem_acquire(SEM_UART);
        tinylibc_printf("CONSUMER: Read '%s' (len=%u, Tick: %u)\n", data, len, system_ticks);
        rtos_sem_release(SEM_UART);
    }
    rtos_sleep_ticks(60); // Simulate work and allow producer to run
}