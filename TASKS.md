# Gazelle RTOS — Tasks & PRs

## Platform Migration

### PR 0: Migrate from ATmega328P to ARM Cortex-M3 (LM3S6965EVB)
**Files:** `Makefile`, `timer.c`, `uart.c`, `rtos.c`, `rtos.h`, `main.c`, `tasks.h`
**New files:** `hw.h`, `startup.c`, `lm3s6965.ld`

Move the project off AVR and onto a 32-bit Cortex-M3 target emulated by QEMU.
This resolves the `system_ticks` race (32-bit reads are atomic on a 32-bit bus),
unlocks PendSV for preemptive context switching, and replaces simavr with the
more capable `qemu-system-arm`.

**Scope:**
- Replace `avr-gcc` toolchain with `arm-none-eabi-gcc`; replace simavr with
  `qemu-system-arm -machine lm3s6965evb -serial stdio`
- Add `lm3s6965.ld` linker script (256KB flash @ `0x00000000`, 64KB SRAM @ `0x20000000`)
- Add `startup.c`: Cortex-M3 vector table, `Reset_Handler` (copies `.data`, zeroes `.bss`)
- Add `hw.h`: SysTick registers, UART0 registers, NVIC, SYSCTL clock gating, GPIO Port A
- Replace `timer.c` Timer1/CTC with SysTick (`SYST_RVR`, `SYST_CSR`); ISR becomes `SysTick_Handler`
- Replace `uart.c` ATmega UART registers with LM3S6965 UART0; ISR becomes `UART0_Handler`
- Replace `sei()`/`cli()` in `rtos.c` with `cpsie i`/`cpsid i`; replace `_delay_ms` busy-wait
  with a tick-based loop using `system_ticks`
- Remove all `#include <avr/io.h>`, `<avr/interrupt.h>`, `<util/delay.h>` from all files

**Note:** `uart_buffer.c`, `ipc.c`, and `tinylibc/` are pure C and require no changes.

---

## Bug Fixes

### PR 1: ~~Fix 32-bit system_ticks race condition~~ *(resolved by PR 0)*
~~On AVR (8-bit), reading or writing a `uint32_t` takes 4 separate load/store instructions.
A Timer1 ISR firing mid-read in the scheduler will corrupt the tick value.~~

Resolved by the Cortex-M3 migration — 32-bit reads/writes are atomic on a 32-bit bus.

---

### PR 2: Fix unsafe task removal during scheduling
**Files:** `rtos.c`

`rtos_remove_task()` shifts the task array in place. If a task removes itself mid-run,
the scheduler's loop index is now pointing at the wrong task. No guard exists against
this scenario.

**Fix:** Instead of shifting the array immediately, mark the task for deferred removal
with a `TASK_ZOMBIE` state, and let the scheduler clean it up at the top of the next
scheduling cycle, before it begins iterating.

---

### PR 3: Clean up dead semaphore API
**Files:** `rtos.h`, `rtos.c`

Semaphore declarations remain in `rtos.h` after the implementation was removed in
commit `1263d5c`. The `TASK_BLOCKED` state in the task state enum is also unused.
This creates misleading API surface.

**Fix:** Remove the semaphore declarations and `TASK_BLOCKED` from the header until
the feature is re-implemented. (See Feature: Semaphores & Blocking below.)

---

## Robustness Improvements

### PR 4: Add UART overflow detection
**Files:** `uart_buffer.c`, `uart_buffer.h`

When the receive ring buffer is full, incoming bytes are silently dropped. On a busy
or noisy serial line this causes invisible data loss.

**Fix:** Add an `overflow` flag to the `UartBuffer` struct and a `uart_buffer_overflow()`
accessor. Set the flag on drop; clear it after the caller reads it. This gives the
application a way to detect and log lost bytes.

---

### PR 5: Add timeout to blocking UART receive
**Files:** `uart.c`, `uart.h`

`uart_receive()` spins forever on `UART0_FR & UART_FR_RXFE` (after PR 0). A disconnected
or unresponsive serial line will stall the entire system, since the cooperative scheduler
cannot run while a task is blocked in a spin-loop.

**Fix:** Add a `timeout_ms` parameter (0 = wait forever for backwards compatibility).
Use `system_ticks` to bound the wait, and change the signature to return `int` so -1
can signal timeout (mirrors standard `getchar` semantics).

---

### PR 6: Audit and fix magic buffer size numbers
**Files:** `uart_buffer.c`, `ipc.c`, `ipc.h`

`uart_buffer.c` hardcodes `64` and the modulo mask directly. `ipc.c` uses the
`IPC_BUFFER_SIZE` and `IPC_DATA_SIZE` constants in some places but not all. A mismatch
between a `#define` and a hardcoded value will silently produce a broken buffer.

**Fix:** Audit every size-dependent expression in these files and replace all literals
with their named constants. Add a `static_assert` (or equivalent macro) where possible
to catch mismatches at compile time.

---

## Code Cleanup

### PR 7: Clean up main.c and types.h
**Files:** `main.c`, `types.h`

- `main.c` contains five commented-out test scenarios that inflate the file and make
  the entry point hard to read. These should live in `tasks/` as separate files (like
  `test_priority_tasks.c`) or be removed entirely. (AVR include cleanup is handled by PR 0.)
- `types.h` is empty. Either add the project's common typedefs here or delete it.

---

## Features

### Semaphores & Blocking (required for preemptive scheduling)
**Files:** `rtos.c`, `rtos.h`

Semaphores were implemented in `db6706c` and removed in `1263d5c` during work toward
a preemptive scheduler. They are listed as a goal in README.md.

**Scope:**
- Re-introduce `Semaphore` type with count, and a waiting-task list (or simple blocked-
  task flag given the 5-task max)
- Implement `rtos_sem_init()`, `rtos_sem_wait()`, `rtos_sem_signal()`
- `rtos_sem_wait()` should transition the calling task to `TASK_BLOCKED` and yield
- `rtos_sem_signal()` should wake the highest-priority blocked waiter
- Re-enable `TASK_BLOCKED` in the task state enum once this is in place

---

### Preemptive scheduling via PendSV + SysTick
**Files:** `rtos.c`, `rtos.h`, `timer.c`, `startup.c`

The existing cooperative scheduler requires tasks to call `rtos_yield()` or
`rtos_sleep()` voluntarily. A preemptive scheduler would forcibly context-switch
on each timer tick, ensuring no task can starve others.

Cortex-M3 is purpose-built for this: SysTick fires the tick ISR which pends PendSV,
and PendSV (lowest-priority exception) performs the lazy context switch after all
other ISRs have returned — the standard ARM RTOS pattern.

**Scope:**
- Allocate a dedicated stack for each task (static arrays in `rtos.c`); the current
  single hardware stack must be per-task
- On SysTick ISR: run scheduler logic, set `SCB->ICSR |= PENDSVSET` to trigger PendSV
- PendSV handler (naked ISR in `startup.c` or `rtos.c`): push/pop `{r4-r11}` and
  swap PSP between outgoing and incoming task stacks
- Reuse the existing priority-based round-robin selection logic unchanged
- Requires PR 0 (Cortex-M3 migration) and semaphores above to land first

---

### Error type enum
**Files:** `types.h` (or new `errors.h`)

Currently, functions return `bool` on failure with no indication of why. A small error
enum would make debugging significantly easier, especially over serial.

**Scope:**
- Define `GazelleError` enum: `ERR_OK`, `ERR_TASK_FULL`, `ERR_INVALID_ARG`,
  `ERR_IPC_FULL`, `ERR_IPC_EMPTY`, `ERR_UART_OVERFLOW`, `ERR_UART_TIMEOUT`, etc.
- Update function signatures in `rtos.h`, `ipc.h`, `uart.h` to return `GazelleError`
  instead of `bool`
- Keep the enum small — even at 256KB flash, there's no reason to bloat it

---

### Memory management
**Files:** new `mem.c/h`

Listed as a future goal in README.md. A full `malloc` is still impractical on a
microcontroller, but a fixed-size block allocator (pool allocator) would be useful
for IPC message buffers and, more importantly, dynamic task stacks for the preemptive
scheduler.

**Scope:**
- Implement a pool allocator with a compile-time-configured block size and pool size
- `mem_alloc()` returns a pointer to a free block or NULL if exhausted
- `mem_free()` returns a block to the pool
- With 64KB SRAM available (vs 2KB on AVR), pool sizes can be meaningfully larger;
  primary use case is dynamic task stacks for the preemptive scheduler
