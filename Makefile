# Target platform
TARGET = gazelle

# Source files
SRC = bsp/startup.c main.c \
      kernel/rtos.c kernel/timer.c kernel/idle_task.c \
      drivers/uart.c drivers/uart_buffer.c drivers/ipc.c \
      tinylibc/conv.c tinylibc/debug.c tinylibc/io.c tinylibc/string.c \
      examples/sensor_task.c examples/display_task.c examples/heartbeat_task.c

# ARM Cortex-M3 toolchain
CC      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE    = arm-none-eabi-size

# Compiler flags
CFLAGS  = -mthumb -mcpu=cortex-m3 -Os -Wall -std=c11

# Linker flags
LDFLAGS = -mthumb -mcpu=cortex-m3 -T lm3s6965.ld -nostartfiles

# Debug build
ifeq ($(DEBUG), 1)
  CFLAGS  += -DDEBUG -g
  BUILD_DIR = debug
else
  BUILD_DIR = release
endif

# Derived paths
OBJ = $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRC))
ELF = $(BUILD_DIR)/$(TARGET).elf
HEX = $(BUILD_DIR)/$(TARGET).hex

# Default target
all: directories $(HEX) size

# Create build directories
directories:
	@mkdir -p $(BUILD_DIR)/bsp
	@mkdir -p $(BUILD_DIR)/kernel
	@mkdir -p $(BUILD_DIR)/drivers
	@mkdir -p $(BUILD_DIR)/tinylibc
	@mkdir -p $(BUILD_DIR)/examples

# Compile
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Link
$(ELF): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

# HEX for reference / flashing real hardware
$(HEX): $(ELF)
	$(OBJCOPY) -O ihex $< $@

# Size report
size: $(ELF)
	$(SIZE) $<

# Run under QEMU
simulate: $(ELF)
	qemu-system-arm -machine lm3s6965evb -nographic -kernel $<

# Clean
clean:
	rm -rf $(BUILD_DIR)

# Host-native unit tests (no QEMU required)
test:
	$(MAKE) -C tests test

# Explicit debug build
debug:
	$(MAKE) DEBUG=1

.PHONY: all directories simulate clean debug size test
