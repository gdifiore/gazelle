#ifndef TASKS_H
#define TASKS_H

#include "../kernel/rtos.h"
#include "../drivers/uart_buffer.h"

void idle_task(void);

void task1(void);
void task2(void);

#endif
