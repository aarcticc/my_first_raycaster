#ifndef RAYCASTER_H
#define RAYCASTER_H

#include <SDL2/SDL.h>
#include "map.h"
#include "texture.h"

// Window and rendering configuration
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define MOVE_SPEED 0.06f
#define ROT_SPEED 0.06f

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

// Graphics system
int init_graphics(Graphics *gfx);
void shutdown_graphics(Graphics *gfx);

// Main rendering
void render_frame(Graphics *gfx, Player *player, int map[MAP_HEIGHT][MAP_WIDTH]);

// Input handling
void handle_input(Player *player, const Uint8 *keystate, int map[MAP_HEIGHT][MAP_WIDTH]);

#endif
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

