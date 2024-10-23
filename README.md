# gazelle
gazelle - byo RTOS project

Targeting Cortex-M3 (STM32)

## Implement List
- [x] tasks/scheduler
- [ ] idle task
- [ ] UART output
- [ ] IPC
- [ ] timing(?)
- [ ] memory management
- [ ] debug/logging (performance?)

## Build
`make`

## Run
`qemu-system-arm -M lm3s6965evb -kernel kernel.elf -nographic`

## Debug?

```
qemu-system-arm -M lm3s6965evb -kernel kernel.elf -nographic -s -S

arm-none-eabi-gdb kernel.elf

target remote :1234
```