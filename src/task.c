#include "../include/task.h"

// Array of tasks
TaskControlBlock taskList[MAX_TASKS];
uint8_t currentTask = 0;

void createTask(void (*taskFunction)(void), uint32_t *stack, uint8_t priority)
{
    static uint32_t taskIDCounter = 0;

    // init task control block
    taskList[taskIDCounter].stackPointer = stack;
    taskList[taskIDCounter].taskID = taskIDCounter;
    taskList[taskIDCounter].priority = priority;
    taskList[taskIDCounter].state = TASK_READY;

    // set up initial stack for the task
    *(--stack) = (uint32_t)taskFunction; // PC
    *(--stack) = 0xFFFFFFFF;             // LR
    taskList[taskIDCounter].stackPointer = stack;

    taskIDCounter++;
}

void switchContextTo(TaskControlBlock *nextTask)
{
    asm volatile(
        "push {r0-r12}\n"            // save registers
        "mrs r0, psp\n"              // get PSP
        "str r0, [%[currentTask]]\n" // save current task's PSP
        "ldr r0, [%[nextTask]]\n"    // load next task's PSP
        "msr psp, r0\n"              // switch to next task's PSP
        "pop {r0-r12}\n"             // restore registers
        "bx lr\n"                    // return from interrupt
        :
        : [currentTask] "r"(&taskList[currentTask].stackPointer),
          [nextTask] "r"(nextTask->stackPointer));
}

void schedule(void)
{
    // start with task 0 if not already running
    if (currentTask >= MAX_TASKS) {
        currentTask = 0;
    }

    while (1) {
        // find next ready task
        uint8_t nextTask = (currentTask + 1) % MAX_TASKS;

        // look for a ready task
        while (nextTask != currentTask) {
            if (taskList[nextTask].state == TASK_READY) {
                break;
            }
            nextTask = (nextTask + 1) % MAX_TASKS;
        }

        // switch to the next task
        if (nextTask != currentTask) {
            uint8_t prevTask = currentTask;
            currentTask = nextTask;
            switchContextTo(&taskList[nextTask]);
        }
    }
}