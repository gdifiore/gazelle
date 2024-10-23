# Toolchain
CC = arm-none-eabi-gcc
CFLAGS = -mcpu=cortex-m3 -mthumb -Wall -Iinclude
LDFLAGS = -T linker_script.ld -nostartfiles

# Directories
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj

# Source files and object files
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_FILES))

# Output
OUTPUT = kernel.elf

# Phony targets
.PHONY: all clean

# Default target
all: $(OUTPUT)

# Link object files
$(OUTPUT): $(OBJ_FILES)
	$(CC) $(OBJ_FILES) -o $@ $(LDFLAGS)

# Compile source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -rf $(OBJ_DIR) $(OUTPUT)
