#ifndef RAYCASTER_H
#define RAYCASTER_H

// Include necessary headers for map data, texture definitions, and SDL graphics library
#include "map.h"
#include "texture.h"
#include <SDL2/SDL.h>

/*
    size = SCREEN_WIDTH x SCREEN_HEIGHT

  16:9:
    640  x 360   // nHD
    1280 x 720   // WXGA
    1360 x 768   // HD
    1366 x 768   // HD
    1600 x 900   // HD+
    1920 x 1080  // FHD
    2048 x 1152  // QWXGA
    2560 x 1440  // QHD
    3840 x 2160  // 4K UHD
    5120 x 2880  // 5K
    6144 x 3456  // 6K
    7680 x 4320  // 8K UHD

  16:10:
    1280 x 800   // WXGA
    1440 x 900   // WXGA+
    1680 x 1050  // WSXGA+
    1920 x 1200  // WUXGA
    2560 x 1600  // WQXGA

  4:3:
    640  x 480   // VGA
    800  x 600   // SVGA
    1024 x 768   // XGA
    1600 x 1200  // UXGA
    2048 x 1536  // QXGA
*/

// Game configuration constants
// SCREEN_WIDTH and SCREEN_HEIGHT define the game window resolution
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
// MOVE_SPEED controls how fast the player moves
#define MOVE_SPEED 0.06f
// ROT_SPEED controls how fast the player rotates
#define ROT_SPEED 0.06f

// Player structure containing position and view direction data
typedef struct {
    float x, y;          // Player's position in 2D space
    float dirX, dirY;    // Direction vector - where player is looking
    float planeX, planeY; // Camera plane - used for raycasting field of view
    float perpWallDist[SCREEN_WIDTH]; // Add z-buffer for sprite rendering
} Player;

// Graphics structure containing SDL rendering components
typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    Uint32 *pixels;
} Graphics;

// Function to initialize SDL and create window/renderer
// Returns 0 on success, non-zero on failure
int init_graphics(Graphics *gfx);

// Function to clean up and free graphics resources
void shutdown_graphics(Graphics *gfx);

// Function to render a single frame of the game
// Takes current player state and map data as input
void render_frame(Graphics *gfx, Player *player, int map[MAP_HEIGHT][MAP_WIDTH]);

// Function to process keyboard input and update player position
// Uses SDL's keyboard state array to determine which keys are pressed
void handle_input(Player *player, const Uint8 *keystate, int map[MAP_HEIGHT][MAP_WIDTH]);

#endif

