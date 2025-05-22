/*******************************************************************************
 * File: ipc.c
 *
 * Description:
 *   Shared memory IPC implementation for the gazelle RTOS.
 *   Provides a circular buffer for inter-task communication, protected by a semaphore.
 *
 * Author: Gabe DiFiore
 * Created: 2025-05-19
 *
 * Target MCU: ATmega328P
 * Toolchain: avr-gcc, avr-libc
 * Platform: Arduino Nano (bare-metal)
 ******************************************************************************/

#include "ipc.h"
#include "types.h"
#include "rtos.h"
#include "tinylibc/tinylibc.h"

static SharedMemory shm;

void ipc_init(void)
{
    shm.head = 0;
    shm.tail = 0;
    shm.count = 0;
    tinylibc_memset(shm.buffer, 0, IPC_BUFFER_SIZE * IPC_DATA_SIZE);
}

bool ipc_write(const char *data, uint8_t len)
{
    // Check if len exceeds IPC_DATA_SIZE (including null terminator)
    if (len >= IPC_DATA_SIZE)
    {
        tinylibc_printf("IPC: Write failed, data length %u >= %u (max including null)\n", len, IPC_DATA_SIZE);
        return false;
    }

    // Check if data is null-terminated within len
    bool null_terminated = false;
    for (uint8_t i = 0; i < len; i++)
    {
        if (data[i] == '\0')
        {
            null_terminated = true;
            break;
        }
    }
    if (!null_terminated)
    {
        tinylibc_printf("IPC: Write failed, data not null-terminated within %u bytes\n", len);
        return false;
    }

    if (shm.count >= IPC_BUFFER_SIZE)
    {
        tinylibc_printf("IPC: Write failed, buffer full\n");
        return false;
    }

    // Copy data to buffer with bounded copy
    tinylibc_strncpy(shm.buffer[shm.head], data, IPC_DATA_SIZE);
    shm.head = (shm.head + 1) % IPC_BUFFER_SIZE;
    shm.count++;

    return true;
}

bool ipc_read(char *data, uint8_t max_len, uint8_t *out_len)
{
    if (shm.count == 0)
    {
        tinylibc_printf("IPC: Read failed, buffer empty\n");
        return false;
    }

    // Compute length of data in buffer
    *out_len = tinylibc_strlen(shm.buffer[shm.tail]);
    if (*out_len >= max_len)
    {
        tinylibc_printf("IPC: Read failed, output buffer too small (%u >= %u)\n", *out_len, max_len);
        return false;
    }

    // Copy data from buffer with bounded copy
    tinylibc_strncpy(data, shm.buffer[shm.tail], max_len);
    shm.tail = (shm.tail + 1) % IPC_BUFFER_SIZE;
    shm.count--;

    return true;
}