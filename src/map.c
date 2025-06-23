#include "map.h"
#include "texture.h" // Add this include for NUM_WALL_TEXTURES
#include <stdio.h>

int map[MAP_HEIGHT][MAP_WIDTH];

int load_map(const char* filename, int map[MAP_HEIGHT][MAP_WIDTH]) {
    FILE* f = fopen(filename, "r");
    if (!f) return 0;
    for (int y = 0; y < MAP_HEIGHT; y++)
        for (int x = 0; x < MAP_WIDTH; x++) {
            int v;
            if (fscanf(f, "%d", &v) != 1) {
                fclose(f);
                return 0;
            }
            // Clamp to valid range
            if (v < 0) v = 0;
            if (v > NUM_WALL_TEXTURES) v = NUM_WALL_TEXTURES;
            map[y][x] = v;
        }
    fclose(f);
    return 1;
}

int save_map(const char* filename, int map[MAP_HEIGHT][MAP_WIDTH]) {
    FILE* f = fopen(filename, "w");
    if (!f) return 0;
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++)
            fprintf(f, "%d ", map[y][x]);
        fprintf(f, "\n");
    }
    fclose(f);
    return 1;
}

int is_valid_position(float x, float y) {
    int ix = (int)x, iy = (int)y;
    if (ix < 0 || ix >= MAP_WIDTH || iy < 0 || iy >= MAP_HEIGHT)
        return 0;
    return map[iy][ix] == 0;
}

void find_nearest_empty_space(float* x, float* y) {
    // Simple implementation: find first empty cell
    for (int iy = 0; iy < MAP_HEIGHT; ++iy) {
        for (int ix = 0; ix < MAP_WIDTH; ++ix) {
            if (map[iy][ix] == 0) {
                *x = ix + 0.5f;
                *y = iy + 0.5f;
                return;
            }
        }
    }
}
