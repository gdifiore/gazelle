#ifndef IPC_H
#define IPC_H

#include "../kernel/types.h"
#include "../kernel/rtos.h"
#include <stdbool.h>

#define IPC_BUFFER_SIZE 16 // Number of slots in the circular buffer
#define IPC_DATA_SIZE 32   // Max size of each data item

typedef struct
{
    char buffer[IPC_BUFFER_SIZE][IPC_DATA_SIZE]; // Circular buffer for data
    uint8_t head;                                // Write index
    uint8_t tail;                                // Read index
    uint8_t count;                               // Number of items in buffer
} SharedMemory;

/*
 * Initialise the IPC message queue.
 *
 * Zeroes the message buffer and resets head, tail, and count to 0.
 */
void ipc_init(void);

/*
 * Write a null-terminated message to the queue.
 *
 * data must be null-terminated within the first len bytes.  len is the number
 * of bytes to search (including the null terminator) and must be less than
 * IPC_DATA_SIZE to leave room for the terminator in the slot.
 *
 * Returns false if:
 *   - len >= IPC_DATA_SIZE
 *   - data is not null-terminated within len bytes
 *   - the buffer is full (count == IPC_BUFFER_SIZE)
 */
GazelleError ipc_write(const char *data, uint8_t len);

/*
 * Read the oldest message from the queue into data.
 *
 * Copies the stored string into data (at most max_len bytes).  *out_len is
 * set to strlen of the stored message.
 *
 * Returns false if the queue is empty or the stored string is too long for
 * the output buffer (*out_len >= max_len).  The message is NOT consumed on
 * failure; a subsequent call with a larger buffer will still find it.
 */
GazelleError ipc_read(char *data, uint8_t max_len, uint8_t *out_len);

#endif // IPC_H
