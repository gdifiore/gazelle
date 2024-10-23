#include "../include/task.h"

void task1()
{
    while (1)
    {
        // Task 1 implementation
    }
}

void task2()
{
    while (1)
    {
        // Task 2 implementation
    }
}

int main(void)
{
    uint32_t task1_stack[128];
    uint32_t task2_stack[128];

    createTask(task1, task1_stack + sizeof(task1_stack) / sizeof(uint32_t), 1);
    createTask(task2, task2_stack + sizeof(task2_stack) / sizeof(uint32_t), 1);

    schedule();

    // should never reach this
    return 0;
}
