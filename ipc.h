#ifndef IPC_H
#define IPC_H

#include "types.h"
#include "rtos.h"
#include "stdbool.h"

#define IPC_BUFFER_SIZE 16 // Small size due to ATmega328P's 2KB SRAM
#define IPC_DATA_SIZE 32   // Max size of each data item

typedef struct
{
    char buffer[IPC_BUFFER_SIZE][IPC_DATA_SIZE]; // Circular buffer for data
    uint8_t head;                                // Write index
    uint8_t tail;                                // Read index
    uint8_t count;                               // Number of items in buffer
} SharedMemory;

void ipc_init(void);
bool ipc_write(const char *data, uint8_t len);
bool ipc_read(char *data, uint8_t max_len, uint8_t *out_len);

#endif // IPC_H