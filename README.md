# gazelle

A minimal RTOS targeting the ARM Cortex-M3, emulated via QEMU.

**Implement List**

- [x] tasks & basic scheduler
- [x] better scheduler — Priority-Based Round Robin
- [x] idle task
- [x] UART output
- [x] IPC — shared memory
- [x] timing
- [ ] semaphores (for preemptive scheduling)
- [ ] preemptive scheduling via PendSV + SysTick
- [ ] memory management
- [ ] enum of error types

## Pre-Requisites

```
sudo apt install gcc-arm-none-eabi qemu-system-arm
```

## How To

`make` → compiles everything

`make simulate` → runs under QEMU (`-machine lm3s6965evb -nographic`, UART → stdio); quit with `Ctrl-A X`, or `pkill qemu-system-arm` from another terminal

`make clean` → removes all build files

`make DEBUG=1` → debug build with `-g`
