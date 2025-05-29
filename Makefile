CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -Iinclude
LDFLAGS = -lSDL2

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