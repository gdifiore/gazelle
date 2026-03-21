# gazelle

A minimal RTOS targeting the ARM Cortex-M3, emulated via QEMU.

## Features

- Cooperative priority-based round-robin scheduler with idle task
- Task sleep, yield, and deferred removal
- Counting semaphores with priority-aware wakeup and task blocking
- UART driver — interrupt-driven RX, polling TX, overflow detection, receive timeout
- IPC via shared memory ring buffer
- SysTick-based system timer
- Host-native unit test harness (`make test`, no QEMU required)

## Pre-Requisites

```
sudo apt install gcc-arm-none-eabi qemu-system-arm
```

## How To

`make` → compiles everything

`make simulate` → runs under QEMU (`-machine lm3s6965evb -nographic`, UART → stdio); quit with `Ctrl-A X`, or `pkill qemu-system-arm` from another terminal

`make test` → builds and runs the host-native unit test suite (no QEMU required)

`make clean` → removes all build files

`make DEBUG=1` → debug build with `-g`
