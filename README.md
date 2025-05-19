# gazelle
BYO RTOS targeting the Arduino Nano's ATmega328 processor

**Implement List**
- [x] tasks & basic scheduler
- [ ] better scheduler
- [x] idle task
- [x] UART output
- [ ] IPC
- [x] timing
- [ ] memory management

## Pre-Requisites
`sudo apt install gcc-avr avr-libc avrdude simavr libsimavr-dev`

## How To
`make` → compiles everything

`make upload` → flashes the code to the Arduino

`make simulate` → simulates RTOS via simavr

`make clean` → removes all build files

## Project Outline
```
gazelle/
├── main.c                         # Entry point: initializes RTOS and starts tasks
├── rtos.[ch]                      # Core RTOS kernel (task management, scheduling, semaphores)
├── timer.[ch]                     # Millisecond tick using Timer1
├── uart.[ch]                      # UART driver with optional ring buffer
├── uart_buffer.[ch]               # Ring buffer implementation for UART receive
├── types.h                        # Minimal custom type definitions
├── tasks/
│   ├── idle_task.c                # Idle task implementation
│   ├── task1.c                    # Demo task 1
│   ├── task2.c                    # Demo task 2
│   └── test_priority_tasks.c      # Various tasks to test scheduling functionality (different priorities, sleep timings, semaphore hold/wait, etc.)
├── tasks.h                        # Task function declarations
├── tinylibc/                      # Minimal libc subset
│   ├── io.[ch]                    # printf-style UART output
│   ├── conv.[ch]                  # itoa/atoi
│   ├── debug.[ch]                 # panic/assert
│   ├── string.[ch]                # strlen, strcmp, strcpy, memset
│   └── tinylibc.h                 # Unified include
├── Makefile                       # Build system
└── README.md                      # This file
```