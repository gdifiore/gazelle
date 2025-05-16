# gazelle
BYO RTOS targeting the Arduino Nano's ATmega328 processor

**Implement List**
- [ ] tasks/scheduler
- [ ] idle task
- [x] UART output
- [ ] IPC
- [ ] timing(?)
- [ ] memory management

## Pre-Requisites
`sudo apt install gcc-avr avr-libc avrdude simavr libsimavr-dev`

## How To
`make` → compiles everything

`make upload` → flashes the code to the Arduino

`make clean` → removes all build files