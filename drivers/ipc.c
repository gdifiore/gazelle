/*******************************************************************************
 * File: ipc.c
 *
 * Description:
 *   Shared memory IPC implementation for the gazelle RTOS.
 *   Provides a circular buffer for inter-task communication.
 *
 * Author: Gabe DiFiore
 * Created: 2025-05-19
 *
 * Target MCU: LM3S6965EVB (ARM Cortex-M3)
 * Toolchain: arm-none-eabi-gcc
 * Platform: QEMU lm3s6965evb
 ******************************************************************************/

#include "ipc.h"
#include "../kernel/types.h"
#include "../tinylibc/tinylibc.h"

_Static_assert(IPC_BUFFER_SIZE >= 1,
               "IPC_BUFFER_SIZE must be at least 1");
_Static_assert(IPC_BUFFER_SIZE <= 256,
               "IPC_BUFFER_SIZE must fit in uint8_t index");
_Static_assert(IPC_DATA_SIZE >= 1,
               "IPC_DATA_SIZE must be at least 1");
_Static_assert(IPC_DATA_SIZE <= 256,
               "IPC_DATA_SIZE must fit in uint8_t length");

static SharedMemory shm;

void ipc_init(void)
{
    shm.head = 0;
    shm.tail = 0;
    shm.count = 0;
    tinylibc_memset(shm.buffer, 0, IPC_BUFFER_SIZE * IPC_DATA_SIZE);
}

GazelleError ipc_write(const char *data, uint8_t len)
{
    // Check if len exceeds IPC_DATA_SIZE (including null terminator)
    if (len >= IPC_DATA_SIZE)
    {
        tinylibc_printf("IPC: Write failed, data length %u >= %u (max including null)\n", len, IPC_DATA_SIZE);
        return ERR_INVALID_ARG;
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
        return ERR_INVALID_ARG;
    }

    if (shm.count >= IPC_BUFFER_SIZE)
    {
        tinylibc_printf("IPC: Write failed, buffer full\n");
        return ERR_IPC_FULL;
    }

    // Copy data to buffer with bounded copy
    tinylibc_strncpy(shm.buffer[shm.head], data, IPC_DATA_SIZE);
    shm.head = (shm.head + 1) % IPC_BUFFER_SIZE;
    shm.count++;

    return ERR_OK;
}

GazelleError ipc_read(char *data, uint8_t max_len, uint8_t *out_len)
{
    if (shm.count == 0)
        return ERR_IPC_EMPTY;

    // Compute length of data in buffer
    *out_len = tinylibc_strlen(shm.buffer[shm.tail]);
    if (*out_len >= max_len)
    {
        tinylibc_printf("IPC: Read failed, output buffer too small (%u >= %u)\n", *out_len, max_len);
        return ERR_INVALID_ARG;
    }

    // Copy data from buffer with bounded copy
    tinylibc_strncpy(data, shm.buffer[shm.tail], max_len);
    shm.tail = (shm.tail + 1) % IPC_BUFFER_SIZE;
    shm.count--;

    return ERR_OK;
}