#ifndef TYPES_H
#define TYPES_H

typedef enum {
    ERR_OK,
    ERR_TASK_FULL,
    ERR_INVALID_ARG,
    ERR_IPC_FULL,
    ERR_IPC_EMPTY,
    ERR_UART_OVERFLOW,
    ERR_UART_TIMEOUT,
    ERR_WOULD_BLOCK,
} GazelleError;

#endif // TYPES_H
