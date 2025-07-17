CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -Iinclude -fopenmp
LDFLAGS = -lSDL2 -lSDL2_image -lm -fopenmp

SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c,bin/%.o,$(SRC))
BIN = raycaster

BIN_DIR = bin

.PHONY: all clean run

all: mkdir_bin $(BIN)

mkdir_bin:
	@mkdir -p $(BIN_DIR)

$(BIN): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

bin/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(BIN)
	./$(BIN)

clean:
	rm -f $(OBJ) $(BIN)
	rm -rf $(BIN_DIR)
	rm -f log_*.txt

# Update mapmaker target to only compile the C version
.PHONY: mapmaker
mapmaker: 
	@mkdir -p bin
	$(CC) $(CFLAGS) src/mapmaker/mapmaker.c -o bin/mapmaker $(LDFLAGS)
	@echo "Map Maker compiled successfully. Run with: ./bin/mapmaker"
	@cd tools/mapmaker_rust && cargo build --release
	@echo "Rust Map Maker compiled successfully. Run with: ./tools/mapmaker_rust/target/release/mapmaker"