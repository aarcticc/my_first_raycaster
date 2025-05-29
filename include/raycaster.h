#ifndef RAYCASTER_H
#define RAYCASTER_H

#include <SDL2/SDL.h>
#include <map.h>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define MAP_WIDTH 16
#define MAP_HEIGHT 16
#define MOVE_SPEED 0.05f
#define ROT_SPEED 0.03f

typedef struct {
    float x, y;         // Player position
    float dirX, dirY;   // Direction vector
    float planeX, planeY; // Camera plane (FOV)
} Player;

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    Uint32 *pixels;
} Graphics;

// Function declarations
int init_graphics(Graphics *gfx);
void shutdown_graphics(Graphics *gfx);
void render_frame(Graphics *gfx, Player *player, int map[MAP_HEIGHT][MAP_WIDTH]);
void handle_input(Player *player, const Uint8 *keystate, int map[MAP_HEIGHT][MAP_WIDTH]);

#endif // RAYCASTER_H

