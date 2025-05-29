#ifndef RAYCASTER_H
#define RAYCASTER_H

#include "map.h"
#include <SDL2/SDL.h>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define MOVE_SPEED 0.05f
#define ROT_SPEED 0.04f

typedef struct {
    float x, y;
    float dirX, dirY;
    float planeX, planeY;
} Player;

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    Uint32 *pixels;
} Graphics;

int init_graphics(Graphics *gfx);
void shutdown_graphics(Graphics *gfx);
void render_frame(Graphics *gfx, Player *player, int map[MAP_HEIGHT][MAP_WIDTH]);
void handle_input(Player *player, const Uint8 *keystate, int map[MAP_HEIGHT][MAP_WIDTH]);

#endif
