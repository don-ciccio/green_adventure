CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -I/opt/homebrew/opt/raylib/include
LIBS = -L/opt/homebrew/opt/raylib/lib -lraylib -lm -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
SRC_DIR = src
OBJ_DIR = bin
TARGET = $(OBJ_DIR)/game

# Source files
SOURCES = $(SRC_DIR)/main.c $(SRC_DIR)/game.c $(SRC_DIR)/player.c $(SRC_DIR)/enemy.c $(SRC_DIR)/camera.c $(SRC_DIR)/renderer.c $(SRC_DIR)/scene.c
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Default target
all: $(TARGET)

# Create target executable
$(TARGET): $(OBJECTS) | $(OBJ_DIR)
	$(CC) $(OBJECTS) -o $@ $(LIBS)

# Compile source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create bin directory if it doesn't exist
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Clean build files
clean:
	rm -rf $(OBJ_DIR)

# Rebuild everything
rebuild: clean all

.PHONY: all clean rebuild