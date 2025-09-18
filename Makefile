################################################################################
# Raycaster Project Makefile
# Version: 1.0
# Author: aarticc
# Last Updated: 1758192892
#
# Description:
#   Build system for the raycaster engine with SDL2 graphics.
#   Supports debug/release builds, custom map loading, and asset verification.
################################################################################

# Build configuration
BUILD_TYPE ?= release

# Compiler settings and flags
CC = gcc
BASE_CFLAGS = -Wall -Wextra -Werror -std=c99 -Iinclude -D_USE_MATH_DEFINES
BASE_LDFLAGS = -lSDL2 -lSDL2_image -lm -fopenmp 

# Build-specific flags
ifeq ($(BUILD_TYPE),debug)
    CFLAGS = $(BASE_CFLAGS) -g -DDEBUG -O0
else
    CFLAGS = $(BASE_CFLAGS) -O2 -DNDEBUG
endif
LDFLAGS = $(BASE_LDFLAGS)

# Directory structure
SRC_DIR = src
BIN_DIR = bin
MAPS_DIR = maps
ASSETS_DIR = assets
REQUIRED_DIRS = $(BIN_DIR) $(MAPS_DIR) $(ASSETS_DIR)

# Source and object files
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c,$(BIN_DIR)/%.o,$(SRC))
BIN = raycaster

# Required asset files
REQUIRED_ASSETS = wall1.png wall2.png wall3.png wall4.png floor.png ceiling.png

# Declare phony targets
.PHONY: all clean deep-clean run dirs check-deps load-map check-assets help debug release check test backup

# Default target
all: check-deps dirs check-assets $(BIN)

# Map loading and verification
load-map:
	@echo "Checking for maps directory..."
	@mkdir -p $(MAPS_DIR)
	@if [ -f "$(MAPS_DIR)/custom_map.json" ]; then \
		echo "✓ Found custom map, ready to load"; \
	else \
		echo "⚠ No custom map found in $(MAPS_DIR)/"; \
		echo "  Please save a map from the mapmaker first"; \
	fi

# Run target with optional map loading
run: $(BIN)
	@if [ -n "$(MAP)" ]; then \
		if [ -f "$(MAP)" ]; then \
			echo "→ Loading custom map: $(MAP)"; \
			cp "$(MAP)" "$(MAPS_DIR)/custom_map.json"; \
			./$(BIN); \
		else \
			echo "✗ Error: Map file $(MAP) not found"; \
			exit 1; \
		fi \
	else \
		echo "→ Running with default map"; \
		./$(BIN); \
	fi

# Enhanced dependency checking
check-deps:
	@echo "→ Checking build dependencies..."
	@command -v $(CC) >/dev/null 2>&1 || { echo "✗ Error: $(CC) compiler not found"; exit 1; }
	@command -v sdl2-config >/dev/null 2>&1 || { echo "✗ Error: SDL2 development files not found"; exit 1; }
	@command -v pkg-config >/dev/null 2>&1 || { echo "✗ Error: pkg-config not found"; exit 1; }
	@pkg-config --exists SDL2_image || { echo "✗ Error: SDL2_image not found"; exit 1; }
	@echo "✓ All dependencies satisfied"

# Directory creation
dirs:
	@mkdir -p $(REQUIRED_DIRS)

# Enhanced build reporting
$(BIN): $(OBJ)
	@echo "→ Linking $(BIN) ($(BUILD_TYPE) build)..."
	@$(CC) -o $@ $^ $(LDFLAGS)
	@echo "✓ Build complete"
	@echo "  Binary size: $$(stat -f%z $@ 2>/dev/null || stat -c%s $@) bytes"

# Object file compilation
$(BIN_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "→ Compiling $<..."
	@$(CC) $(CFLAGS) -c $< -o $@ || \
	(echo "✗ Error compiling $<" && \
	echo "  Command: $(CC) $(CFLAGS) -c $< -o $@" && \
	$(CC) $(CFLAGS) -c $< -o $@ 2>&1 && exit 1)
	@echo "✓ Successfully compiled $<"

# Enhanced asset verification
check-assets:
	@echo "→ Verifying assets..."
	@mkdir -p $(ASSETS_DIR)
	@for asset in $(REQUIRED_ASSETS); do \
		if [ ! -f "$(ASSETS_DIR)/$$asset" ]; then \
			echo "✗ Error: $$asset missing from $(ASSETS_DIR)/"; \
			echo "  Please ensure all required assets are present"; \
			exit 1; \
		fi \
	done
	@echo "✓ All assets verified"

# Backup utility
backup:
	@echo "→ Creating backup..."
	@tar -czf raycaster_backup_$$(date +%Y%m%d_%H%M%S).tar.gz \
		$(SRC_DIR) $(ASSETS_DIR) $(MAPS_DIR) Makefile
	@echo "✓ Backup created"

# Cleanup targets
clean:
	@echo "→ Cleaning build files..."
	@rm -f $(OBJ) $(BIN)
	@rm -rf $(BIN_DIR)
	@rm -f log_*.txt
	@find . -name "*.o" -delete
	@find . -name "*.d" -delete
	@echo "✓ Clean complete"

deep-clean: clean
	@echo "Performing deep clean..."
	@rm -rf $(MAPS_DIR)/*.json
	@echo "✓ Deep clean complete (maps and build files removed)"

help:
	@echo "Available targets:"
	@echo "  make          - Build release version"
	@echo "  make debug    - Build debug version"
	@echo "  make release  - Build optimized version"
	@echo "  make clean    - Remove build files"
	@echo "  make run     - Run the program"
	@echo "  make backup   - Create backup of source files"
	@echo "  make check   - Verify build environment"

debug:
	@$(MAKE) BUILD_TYPE=debug

release:
	@$(MAKE) BUILD_TYPE=release
