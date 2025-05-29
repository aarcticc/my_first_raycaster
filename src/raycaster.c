#include "raycaster.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Initialize SDL graphics and allocate pixel buffer
int init_graphics(Graphics *gfx) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return -1;

    gfx->window = SDL_CreateWindow("Raycaster",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (!gfx->window) return -1;

    gfx->renderer = SDL_CreateRenderer(gfx->window, -1, SDL_RENDERER_ACCELERATED);
    gfx->texture = SDL_CreateTexture(gfx->renderer,
        SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
        SCREEN_WIDTH, SCREEN_HEIGHT);

    gfx->pixels = malloc(sizeof(Uint32) * SCREEN_WIDTH * SCREEN_HEIGHT);
    if (!gfx->pixels) return -1;

    return 0;
}

// Clean up SDL resources and free pixel buffer
void shutdown_graphics(Graphics *gfx) {
    free(gfx->pixels);
    SDL_DestroyTexture(gfx->texture);
    SDL_DestroyRenderer(gfx->renderer);
    SDL_DestroyWindow(gfx->window);
    SDL_Quit();
}

// Render a single frame using raycasting
// gfx: graphics context, player: player state, map: 2D map array
void render_frame(Graphics *gfx, Player *player, int map[MAP_HEIGHT][MAP_WIDTH]) {
    // === Fill ceiling and floor ===
    for (int y = 0; y < SCREEN_HEIGHT / 2; ++y) {
        for (int x = 0; x < SCREEN_WIDTH; ++x) {
            gfx->pixels[y * SCREEN_WIDTH + x] = 0xFF202020; // ceiling (dark gray)
            gfx->pixels[(y + SCREEN_HEIGHT / 2) * SCREEN_WIDTH + x] = 0xFF606060; // floor (gray)
        }
    }

    // === Begin raycasting ===
    for (int x = 0; x < SCREEN_WIDTH; x++) {
        float cameraX = 2.0f * x / SCREEN_WIDTH - 1.0f;
        float rayDirX = player->dirX + player->planeX * cameraX;
        float rayDirY = player->dirY + player->planeY * cameraX;

        int mapX = (int)player->x;
        int mapY = (int)player->y;

        float sideDistX, sideDistY;
        float deltaDistX = (rayDirX == 0) ? 1e30 : fabsf(1.0f / rayDirX);
        float deltaDistY = (rayDirY == 0) ? 1e30 : fabsf(1.0f / rayDirY);

        float perpWallDist;

        int stepX, stepY;
        int hit = 0;
        int side;

        if (rayDirX < 0) {
            stepX = -1;
            sideDistX = (player->x - mapX) * deltaDistX;
        } else {
            stepX = 1;
            sideDistX = (mapX + 1.0f - player->x) * deltaDistX;
        }

        if (rayDirY < 0) {
            stepY = -1;
            sideDistY = (player->y - mapY) * deltaDistY;
        } else {
            stepY = 1;
            sideDistY = (mapY + 1.0f - player->y) * deltaDistY;
        }

        // DDA
        while (!hit) {
            if (sideDistX < sideDistY) {
                sideDistX += deltaDistX;
                mapX += stepX;
                side = 0;
            } else {
                sideDistY += deltaDistY;
                mapY += stepY;
                side = 1;
            }
            if (map[mapY][mapX] > 0) hit = 1;
        }

        if (side == 0)
            perpWallDist = (mapX - player->x + (1 - stepX) / 2.0f) / rayDirX;
        else
            perpWallDist = (mapY - player->y + (1 - stepY) / 2.0f) / rayDirY;

        if (perpWallDist < 0.01f) perpWallDist = 0.01f;

        int lineHeight = (int)(SCREEN_HEIGHT / perpWallDist);
        if (lineHeight > SCREEN_HEIGHT * 10) continue;

        int drawStart = -lineHeight / 2 + SCREEN_HEIGHT / 2;
        if (drawStart < 0) drawStart = 0;

        int drawEnd = lineHeight / 2 + SCREEN_HEIGHT / 2;
        if (drawEnd >= SCREEN_HEIGHT) drawEnd = SCREEN_HEIGHT - 1;


        Uint32 color = (side == 0) ? 0xFFAAAAAA : 0xFF888888;

        for (int y = drawStart; y < drawEnd; y++) {
        gfx->pixels[y * SCREEN_WIDTH + x] = color;
    }
    }

    // === Present frame ===
    SDL_UpdateTexture(gfx->texture, NULL, gfx->pixels, SCREEN_WIDTH * sizeof(Uint32));
    SDL_RenderCopy(gfx->renderer, gfx->texture, NULL, NULL);
    SDL_RenderPresent(gfx->renderer);
}

// Handle player movement and rotation based on keyboard input
void handle_input(Player *player, const Uint8 *keystate, int map[MAP_HEIGHT][MAP_WIDTH]) {
    float newX, newY;

    // Move forward
    if (keystate[SDL_SCANCODE_UP]) {
        newX = player->x + player->dirX * MOVE_SPEED;
        newY = player->y + player->dirY * MOVE_SPEED;

        if (map[(int)newY][(int)player->x] == 0)
            player->y = newY;

        if (map[(int)player->y][(int)newX] == 0)
            player->x = newX;
    }

    // Move backward
    if (keystate[SDL_SCANCODE_DOWN]) {
        newX = player->x - player->dirX * MOVE_SPEED;
        newY = player->y - player->dirY * MOVE_SPEED;

        if (map[(int)newY][(int)player->x] == 0)
            player->y = newY;

        if (map[(int)player->y][(int)newX] == 0)
            player->x = newX;
    }

    // Rotate left
    if (keystate[SDL_SCANCODE_LEFT]) {
        float oldDirX = player->dirX;
        float oldPlaneX = player->planeX;
        player->dirX = player->dirX * cos(ROT_SPEED) - player->dirY * sin(ROT_SPEED);
        player->dirY = oldDirX * sin(ROT_SPEED) + player->dirY * cos(ROT_SPEED);
        player->planeX = player->planeX * cos(ROT_SPEED) - player->planeY * sin(ROT_SPEED);
        player->planeY = oldPlaneX * sin(ROT_SPEED) + player->planeY * cos(ROT_SPEED);
    }

    // Rotate right
    if (keystate[SDL_SCANCODE_RIGHT]) {
        float oldDirX = player->dirX;
        float oldPlaneX = player->planeX;
        player->dirX = player->dirX * cos(-ROT_SPEED) - player->dirY * sin(-ROT_SPEED);
        player->dirY = oldDirX * sin(-ROT_SPEED) + player->dirY * cos(-ROT_SPEED);
        player->planeX = player->planeX * cos(-ROT_SPEED) - player->planeY * sin(-ROT_SPEED);
        player->planeY = oldPlaneX * sin(-ROT_SPEED) + player->planeY * cos(-ROT_SPEED);
    }
}
