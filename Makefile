# Compiler and flags
CC := gcc
CFLAGS := -Wall -Wextra -O2 -Wno-unused-function
LDFLAGS := -lm

# Add SDL2 flags
CFLAGS += $(shell sdl2-config --cflags)
LDFLAGS += $(shell sdl2-config --libs)

# Source files
SRC_DIR := src
SRC_FILES := $(shell find $(SRC_DIR) -type f -name "*.c")
ALL_SRC := main.c $(SRC_FILES)
OBJ_FILES := $(ALL_SRC:.c=.o)

# Output binary
TARGET := main

# Default rule
all: $(TARGET)

# Link all objects into final binary
$(TARGET): $(OBJ_FILES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile .c to .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f $(OBJ_FILES) $(TARGET)
