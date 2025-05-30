#include "raycaster.h"
#include "map.h"
#include <stdlib.h>
#include <math.h>

// graphics initialisation and startup
int init_graphics(Graphics *gfx) {
    gfx->window = SDL_CreateWindow("Raycaster", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (!gfx->window) return -1;
    gfx->renderer = SDL_CreateRenderer(gfx->window, -1, SDL_RENDERER_ACCELERATED);
    gfx->texture = SDL_CreateTexture(gfx->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
    gfx->pixels = malloc(sizeof(Uint32) * SCREEN_WIDTH * SCREEN_HEIGHT);
    return (gfx->pixels) ? 0 : -1;
}

// shutdown graphics, used line 37 in main.c
void shutdown_graphics(Graphics *gfx) {
    free(gfx->pixels);
    SDL_DestroyTexture(gfx->texture);
    SDL_DestroyRenderer(gfx->renderer);
    SDL_DestroyWindow(gfx->window);
    SDL_Quit();
}

// rendering loop
void render_frame(Graphics *gfx, Player *player, int map[MAP_HEIGHT][MAP_WIDTH]) {
    for (int y = 0; y < SCREEN_HEIGHT / 2; ++y) {
        for (int x = 0; x < SCREEN_WIDTH; ++x) {
            gfx->pixels[y * SCREEN_WIDTH + x] = 0xFF202020;     // 0xsomething is always a data adress on your harddrive
            gfx->pixels[(y + SCREEN_HEIGHT / 2) * SCREEN_WIDTH + x] = 0xFF606060;
        }
    }

    // raycasting loop
    for (int x = 0; x < SCREEN_WIDTH; x++) {
        float cameraX = 2.0f * x / SCREEN_WIDTH - 1.0f;
        float rayDirX = player->dirX + player->planeX * cameraX;
        float rayDirY = player->dirY + player->planeY * cameraX;

        int mapX = (int)player->x;
        int mapY = (int)player->y;

        float deltaDistX = (rayDirX == 0) ? 1e30 : fabsf(1 / rayDirX);
        float deltaDistY = (rayDirY == 0) ? 1e30 : fabsf(1 / rayDirY);
        float sideDistX, sideDistY;

        int stepX = (rayDirX < 0) ? -1 : 1;
        int stepY = (rayDirY < 0) ? -1 : 1;
        sideDistX = (rayDirX < 0) ? (player->x - mapX) * deltaDistX : (mapX + 1.0f - player->x) * deltaDistX;
        sideDistY = (rayDirY < 0) ? (player->y - mapY) * deltaDistY : (mapY + 1.0f - player->y) * deltaDistY;

        int hit = 0, side;
        while (!hit) {
            if (sideDistX < sideDistY) {
                sideDistX += deltaDistX; mapX += stepX; side = 0;
            } else {
                sideDistY += deltaDistY; mapY += stepY; side = 1;
            }
            if (map[mapY][mapX] > 0) hit = 1;
        }

        float perpWallDist = (side == 0) ? (mapX - player->x + (1 - stepX) / 2.0f) / rayDirX
                                         : (mapY - player->y + (1 - stepY) / 2.0f) / rayDirY;
        int lineHeight = (int)(SCREEN_HEIGHT / perpWallDist);
        int drawStart = -lineHeight / 2 + SCREEN_HEIGHT / 2;
        int drawEnd = lineHeight / 2 + SCREEN_HEIGHT / 2;
        if (drawStart < 0) drawStart = 0;
        if (drawEnd >= SCREEN_HEIGHT) drawEnd = SCREEN_HEIGHT - 1;

        Uint32 color = (side == 0) ? 0xFFAAAAAA : 0xFF888888;
        for (int y = drawStart; y < drawEnd; y++) {
            gfx->pixels[y * SCREEN_WIDTH + x] = color;
        }
    }

    //assigning tasks to the gfx
    SDL_UpdateTexture(gfx->texture, NULL, gfx->pixels, SCREEN_WIDTH * sizeof(Uint32));
    SDL_RenderClear(gfx->renderer);
    SDL_RenderCopy(gfx->renderer, gfx->texture, NULL, NULL);
    SDL_RenderPresent(gfx->renderer);
}

// input handling for arrow keys
void handle_input(Player *player, const Uint8 *keystate, int map[MAP_HEIGHT][MAP_WIDTH]) {
    float newX, newY;
    if (keystate[SDL_SCANCODE_UP]) {
        newX = player->x + player->dirX * MOVE_SPEED;
        newY = player->y + player->dirY * MOVE_SPEED;
        if (map[(int)newY][(int)player->x] == 0) player->y = newY;
        if (map[(int)player->y][(int)newX] == 0) player->x = newX;
    }
    if (keystate[SDL_SCANCODE_DOWN]) {
        newX = player->x - player->dirX * MOVE_SPEED;
        newY = player->y - player->dirY * MOVE_SPEED;
        if (map[(int)newY][(int)player->x] == 0) player->y = newY;
        if (map[(int)player->y][(int)newX] == 0) player->x = newX;
    }
    if (keystate[SDL_SCANCODE_LEFT]) {
        float oldDirX = player->dirX;
        player->dirX = player->dirX * cos(ROT_SPEED) - player->dirY * sin(ROT_SPEED);
        player->dirY = oldDirX * sin(ROT_SPEED) + player->dirY * cos(ROT_SPEED);
        float oldPlaneX = player->planeX;
        player->planeX = player->planeX * cos(ROT_SPEED) - player->planeY * sin(ROT_SPEED);
        player->planeY = oldPlaneX * sin(ROT_SPEED) + player->planeY * cos(ROT_SPEED);
    }
    if (keystate[SDL_SCANCODE_RIGHT]) {
        float oldDirX = player->dirX;
        player->dirX = player->dirX * cos(-ROT_SPEED) - player->dirY * sin(-ROT_SPEED);
        player->dirY = oldDirX * sin(-ROT_SPEED) + player->dirY * cos(-ROT_SPEED);
        float oldPlaneX = player->planeX;
        player->planeX = player->planeX * cos(-ROT_SPEED) - player->planeY * sin(-ROT_SPEED);
        player->planeY = oldPlaneX * sin(-ROT_SPEED) + player->planeY * cos(-ROT_SPEED);
    }
}