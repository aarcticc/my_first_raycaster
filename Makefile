CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -Iinclude -fopenmp
LDFLAGS = -lSDL2 -lSDL2_image -lm -fopenmp

SRC = $(wildcard src/*.c)
BIN_DIR = bin

RAYCASTER_OBJ = bin/main.o bin/map.o bin/raycaster.o bin/texture.o
MAP_EDITOR_OBJ = bin/map_editor.o bin/map.o bin/texture.o

.PHONY: all clean run

all: mkdir_bin raycaster map_editor

mkdir_bin:
	@mkdir -p $(BIN_DIR)

raycaster: $(RAYCASTER_OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

map_editor: $(MAP_EDITOR_OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

bin/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

run: raycaster
	./raycaster

clean:
	rm -f bin/*.o raycaster map_editor
	rm -rf $(BIN_DIR)