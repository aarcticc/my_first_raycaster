# C++ Project Makefile
# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -O2 -Iinclude
LDFLAGS = -lSDL2 -lSDL2_image -lm

# Project structure
SRC_DIR = src
INC_DIR = include
BIN_DIR = bin
TARGET = raycaster

# Source files and object files
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(BIN_DIR)/%.o,$(SOURCES))

# Default target
.PHONY: all clean

all: $(TARGET)

# Create binary directory if it doesn't exist
$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

# Link the final executable
$(TARGET): $(BIN_DIR) $(OBJECTS)
	@echo "Linking $(TARGET)..."
	$(CXX) -o $@ $(OBJECTS) $(LDFLAGS)
	@echo "Build complete: $(TARGET)"

# Compile source files to object files
$(BIN_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BIN_DIR)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	rm -rf $(BIN_DIR)
	rm -f $(TARGET)
	rm -f log_*.txt
	@echo "Clean complete"