# Makefile for Snake Game (Windows MinGW + raylib)

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -std=c99 -O2 -IC:/raylib/include

# Linker flags
LDFLAGS = -LC:/raylib/lib -lraylib -lopengl32 -lgdi32 -lwinmm

# Target executable
TARGET = snake_game.exe

# Source files
SOURCES = main.c game.c snake.c food.c collision.c renderer.c utils.c
OBJECTS = $(SOURCES:.c=.o)
HEADER = snake_game.h

# Default target
all: $(TARGET)

# Link object files to create executable
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo "Build complete: $(TARGET)"

# Compile source files to object files
%.o: %.c $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files (Windows)
clean:
	del /F /Q $(OBJECTS) $(TARGET)
	@echo "Clean complete"

# Rebuild from scratch
rebuild: clean all

# Run the game
run: $(TARGET)
	.\$(TARGET)

.PHONY: all clean rebuild run