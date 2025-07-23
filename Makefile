CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c99 -O2 -Iinclude -fopenmp -g
LDFLAGS = -lSDL2 -lSDL2_image -lm -fopenmp -ljson-c

# Source files and object files
SRC_DIR = src
BIN_DIR = bin
MAPS_DIR = maps
TOOLS_DIR = tools
ASSETS_DIR = assets

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c,$(BIN_DIR)/%.o,$(SRC))
BIN = raycaster

# Directories
REQUIRED_DIRS = $(BIN_DIR) $(MAPS_DIR) $(ASSETS_DIR)

.PHONY: all clean run dirs mapmaker check-deps

all: check-deps dirs $(BIN) mapmaker

check-deps:
	@echo "Checking dependencies..."
	@which sdl2-config > /dev/null || (echo "Error: SDL2 development files not found"; exit 1)
	@ldconfig -p | grep libjson-c > /dev/null || (echo "Error: json-c library not found"; exit 1)
	@pkg-config --exists SDL2_image || (echo "Error: SDL2_image not found"; exit 1)
	@echo "All dependencies OK"

dirs:
	@mkdir -p $(REQUIRED_DIRS)
	@mkdir -p $(TOOLS_DIR)

$(BIN): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

$(BIN_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

mapmaker:
	@echo "Checking Python and required modules..."
	@python3 -c "import tkinter" 2>/dev/null || (echo "Error: tkinter module not found. Install with: sudo apt-get install python3-tk"; exit 1)
	@if [ ! -f $(TOOLS_DIR)/mapmaker.py ]; then \
		echo "Error: mapmaker.py not found. Creating default mapmaker..."; \
		cp tools/mapmaker.py $(TOOLS_DIR)/mapmaker.py 2>/dev/null || \
		echo "#!/usr/bin/env python3" > $(TOOLS_DIR)/mapmaker.py && \
		cat tools/mapmaker.py >> $(TOOLS_DIR)/mapmaker.py; \
	fi
	@chmod +x $(TOOLS_DIR)/mapmaker.py
	@echo "Mapmaker tool ready"

run: $(BIN)
	./$(BIN)

clean:
	rm -f $(OBJ) $(BIN)
	rm -rf $(BIN_DIR)
	rm -f log_*.txt
	rm -rf $(MAPS_DIR)/*

# Assets directory check target
.PHONY: check-assets
check-assets:
	@echo "Checking required assets..."
	@test -f $(ASSETS_DIR)/wall1.png || (echo "Error: wall1.png missing"; exit 1)
	@test -f $(ASSETS_DIR)/wall2.png || (echo "Error: wall2.png missing"; exit 1)
	@test -f $(ASSETS_DIR)/wall3.png || (echo "Error: wall3.png missing"; exit 1)
	@test -f $(ASSETS_DIR)/wall4.png || (echo "Error: wall4.png missing"; exit 1)
	@test -f $(ASSETS_DIR)/floor.png || (echo "Error: floor.png missing"; exit 1)
	@test -f $(ASSETS_DIR)/ceiling.png || (echo "Error: ceiling.png missing"; exit 1)
	@echo "All assets OK"

# Add assets check to main build
all: check-assets