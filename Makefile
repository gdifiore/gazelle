# MCU and programmer settings
MCU = atmega328p
F_CPU = 16000000UL
PROGRAMMER = arduino
PORT = /dev/ttyUSB0
BAUD = 115200

# Project name
TARGET = gazelle

# Source files
SRC = main.c rtos.c uart.c tinylibc/conv.c tinylibc/debug.c tinylibc/io.c tinylibc/string.c tasks/task1.c tasks/task2.c

# Compiler and tools
CC = avr-gcc
OBJCOPY = avr-objcopy
SIZE = avr-size
AVRDUDE = avrdude

# Base compiler flags
CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall -std=c11
LDFLAGS = -mmcu=$(MCU)

# Debug build handling
ifeq ($(DEBUG), 1)
  # Assuming simavr is installed in ~/simavr or /usr/local/include/simavr
  # Update these paths according to your simavr installation
  SIMAVR_PATH = $(shell if [ -d ~/simavr ]; then echo ~/simavr; elif [ -d /usr/local/include/simavr ]; then echo /usr/local/include/simavr; else echo /usr/include/simavr; fi)

  CFLAGS += -DDEBUG -g -I$(SIMAVR_PATH)
  BUILD_DIR = debug
else
  BUILD_DIR = release
endif

# Object files with build directory
OBJ = $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRC))

# Output files
HEX = $(BUILD_DIR)/$(TARGET).hex
ELF = $(BUILD_DIR)/$(TARGET).elf

# Default target
all: directories $(HEX) size

# Create build directories
directories:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BUILD_DIR)/tasks

# Compile source files to object files
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Link object files to ELF
$(ELF): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

# Convert ELF to HEX
$(HEX): $(ELF)
	$(OBJCOPY) -O ihex -R .eeprom $< $@

# Display size information
size: $(ELF)
	$(SIZE) --format=avr --mcu=$(MCU) $<

# Flash the device
upload: $(HEX)
	$(AVRDUDE) -c $(PROGRAMMER) -p $(MCU) -P $(PORT) -b $(BAUD) -U flash:w:$(HEX)

# Simulate using simavr
simulate: $(ELF)
	simavr -m $(MCU) -f $(F_CPU) $<

# Simulate with tracing
simulate-trace: $(ELF)
	simavr -m $(MCU) -f $(F_CPU) -v $<

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)

# Target to explicitly build in debug mode
debug:
	$(MAKE) DEBUG=1

.PHONY: all directories upload simulate simulate-trace clean debug size show-simavr-path