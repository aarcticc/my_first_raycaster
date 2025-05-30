#ifndef RAYCASTER_H
#define RAYCASTER_H

#include "map.h"
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

// only use resolution values listed above
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
