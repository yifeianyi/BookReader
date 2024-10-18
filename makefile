# Compiler
CC := gcc

# Directories
SRC_DIR := src
INC_DIR := inc
OBJ_DIR := obj
BIN_DIR := bin

# Compiler flags
CFLAGS := -Wall -I$(INC_DIR)

# Source files
SRCS := $(wildcard $(SRC_DIR)/*.c)

# Object files
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

# Output binary
TARGET := $(BIN_DIR)/BookReader

# Create directories if they don't exist
$(shell mkdir -p $(OBJ_DIR) $(BIN_DIR))

# Default rule
all: $(TARGET)

# Link object files to create the binary
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@

# Compile each .c file to .o file
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up the build files
clean:
	rm -rf $(OBJ_DIR)/*.o $(TARGET)  $(BIN_DIR)

# Phony targets
.PHONY: all clean run

run:
	$(TARGET)
