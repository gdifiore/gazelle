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

__bool ipc_write(const char *data, uint8_t len)
{
    if (len > IPC_DATA_SIZE)
    {
        tinylibc_printf("IPC: Write failed, data too large (%u > %u)\n", len, IPC_DATA_SIZE);
        return false;
    }

    if (shm.count >= IPC_BUFFER_SIZE)
    {
        tinylibc_printf("IPC: Write failed, buffer full\n");
        return false;
    }

    // Copy data to buffer
    tinylibc_strcpy(shm.buffer[shm.head], data);
    shm.head = (shm.head + 1) % IPC_BUFFER_SIZE;
    shm.count++;

    return true;
}

__bool ipc_read(char *data, uint8_t max_len, uint8_t *out_len)
{

    if (shm.count == 0)
    {
        tinylibc_printf("IPC: Read failed, buffer empty\n");
        return false;
    }

    // Copy data from buffer
    *out_len = tinylibc_strlen(shm.buffer[shm.tail]);
    if (*out_len > max_len)
    {
        tinylibc_printf("IPC: Read failed, output buffer too small (%u > %u)\n", *out_len, max_len);
        return false;
    }

    tinylibc_strcpy(data, shm.buffer[shm.tail]);
    shm.tail = (shm.tail + 1) % IPC_BUFFER_SIZE;
    shm.count--;

    return true;
}