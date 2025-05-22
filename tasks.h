#ifndef TASKS_H
#define TASKS_H

#include "rtos.h"
#include <avr/io.h>
#include <util/delay.h>

void idle_task(void);

void task1(void);
void task2(void);

// BEGIN Scheduling Test Tasks
void high_prio_task_A(void);
void high_prio_task_B(void);

void medium_prio_task_X(void);
void medium_prio_task_Y(void);
void medium_prio_task_Z(void);

void low_prio_task_1(void);

void sem_holder_task(void);
void sem_waiter_task(void);

// IPC Test Tasks
void ipc_producer_task(void);
void ipc_consumer_task(void);

// END Scheduling Test Tasks

// UART Input Test Task
void uart_input_task(void);

#endif
