/*******************************************************************************
 * File: main.c
 *
 * Description:
 *   Main file for starting RTOS execution. Initializes the system,
 *   creates tasks, and starts the RTOS scheduler.
 *
 * Author: Gabe DiFiore
 * Created: 2025-05-08
 *
 * Target MCU: ATmega328P
 * Toolchain: avr-gcc, avr-libc
 * Platform: Arduino Nano (bare-metal)
 ******************************************************************************/

#include "rtos.h"
#include "tasks.h"
#include "uart.h"
#include "ipc.h"
#include "tinylibc/tinylibc.h"
#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
    rtos_init();

    uart_init(); // Initialize UART and enable global interrupts
    ipc_init();  // Initialize IPC via shared memory

    // rtos_create_task(task1, MEDIUM);
    // rtos_create_task(task2, MEDIUM);

    /*
    Test Scenario 1: Round-Robin at Same Priority (Medium)

    Expected: You should see M_X, M_Y, M_Z appearing in a cycle.
    If they all sleep, the idle task might run in between.
    The order M_X -> M_Y -> M_Z -> M_X ... (when they are ready) should be evident.

    rtos_create_task(medium_prio_task_X, MEDIUM);
    rtos_create_task(medium_prio_task_Y, MEDIUM);
    rtos_create_task(medium_prio_task_Z, MEDIUM);
    */

    /*
    Test Scenario 2: Priority Adherence

    Expected: H_A should run. M_X should only run when H_A is sleeping.
    L_1 should only run when both H_A and M_X are sleeping.

    rtos_create_task(high_prio_task_A, HIGH);
    rtos_create_task(medium_prio_task_X, MEDIUM);
    rtos_create_task(low_prio_task_1, LOW);
    */

    /*
    Test Scenario 3: Mixed Priorities with Round-Robin

    Expected: H_A and H_B should round-robin.
    M_X and M_Y should only run when both H_A and H_B are sleeping, and then M_X and M_Y should round-robin.

    rtos_create_task(high_prio_task_A, HIGH);
    rtos_create_task(high_prio_task_B, HIGH);
    rtos_create_task(medium_prio_task_X, MEDIUM);
    rtos_create_task(medium_prio_task_Y, MEDIUM);
    */

    /*
    Test Scenario 5: Idle Task Verification

    Expected: After H_A runs and goes to sleep, you should see "IDLE" printed repeatedly (based on its rtos_sleep_ticks(1)) until H_A wakes up.

    rtos_create_task(high_prio_task_A, HIGH); // This task sleeps for a while
    */

    /*
    Test Scenario 6: Shared Memory IPC

    Expected: Producer writes data to the shared buffer, Consumer reads it.
    Both tasks run at HIGH priority to show round-robin behavior when ready.
    Output should show interleaved PRODUCER and CONSUMER messages.

    rtos_create_task(ipc_producer_task, HIGH);
    rtos_create_task(ipc_consumer_task, HIGH);
    */
    rtos_start();

    return 0;
}