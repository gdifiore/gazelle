#ifndef TASK_H
#define TASK_H

#include <stdint.h>

// task states
enum TASK_STATES
{
    TASK_READY = 0,
    TASK_RUNNING,
    TASK_BLOCKED
};

#define MAX_TASKS 5

typedef struct
{
    uint32_t *stackPointer;
    uint32_t taskID;
    uint8_t priority;
    uint8_t state;
} TaskControlBlock;

void createTask(void (*taskFunction)(void), uint32_t *stack, uint8_t priority);
void schedule(void);
void switchContextTo(TaskControlBlock *nextTask);

extern TaskControlBlock taskList[MAX_TASKS];
extern uint8_t currentTask;

#endif // TASK_H
