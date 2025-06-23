#ifndef MAP_H
#define MAP_H

#define MAP_WIDTH 20
#define MAP_HEIGHT 20

extern int map[MAP_HEIGHT][MAP_WIDTH];

void find_nearest_empty_space(float* x, float* y);
int is_valid_position(float x, float y);

#endif