CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c99 -O2 -Iinclude -fopenmp -g -D_USE_MATH_DEFINES
LDFLAGS = -lSDL2 -lSDL2_image -lm -fopenmp

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
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) -c $< -o $@ || \
	(echo "\033[1;31mError compiling $<\033[0m" && \
	echo "Command: $(CC) $(CFLAGS) -c $< -o $@" && \
	$(CC) $(CFLAGS) -c $< -o $@ 2>&1 && exit 1)
	@echo "\033[1;32mSuccessfully compiled $<\033[0m"

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
