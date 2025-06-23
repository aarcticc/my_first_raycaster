#include "raycaster.h"
#include <math.h>
#include <stdlib.h>
#include <omp.h>

int init_graphics(Graphics *gfx) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return -1;
    gfx->window = SDL_CreateWindow("Raycaster", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (!gfx->window) return -1;
    gfx->renderer = SDL_CreateRenderer(gfx->window, -1, SDL_RENDERER_ACCELERATED);
    if (!gfx->renderer) return -1;
    gfx->texture = SDL_CreateTexture(gfx->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
    gfx->pixels = malloc(sizeof(Uint32) * SCREEN_WIDTH * SCREEN_HEIGHT);
    return (gfx->pixels) ? 0 : -1;
}

void shutdown_graphics(Graphics *gfx) {
    free(gfx->pixels);
    SDL_DestroyTexture(gfx->texture);
    SDL_DestroyRenderer(gfx->renderer);
    SDL_DestroyWindow(gfx->window);
    SDL_Quit();
}

void render_frame(Graphics *gfx, Player *player, int map[MAP_HEIGHT][MAP_WIDTH]) {
    // Floor and ceiling
    float posZ = SCREEN_HEIGHT / 2.0f;
    float rayDirX0 = player->dirX - player->planeX;
    float rayDirY0 = player->dirY - player->planeY;
    float rayDirX1 = player->dirX + player->planeX;
    float rayDirY1 = player->dirY + player->planeY;

    #pragma omp parallel for schedule(dynamic, 16)
    for(int y = SCREEN_HEIGHT/2; y < SCREEN_HEIGHT; y++) {
        float rowDistance = posZ / (y - SCREEN_HEIGHT/2);
        float floorStepX = rowDistance * (rayDirX1 - rayDirX0) / SCREEN_WIDTH;
        float floorStepY = rowDistance * (rayDirY1 - rayDirY0) / SCREEN_WIDTH;
        float floorX = player->x + rowDistance * rayDirX0;
        float floorY = player->y + rowDistance * rayDirY0;
        for(int x = 0; x < SCREEN_WIDTH; ++x) {
            int cellX = (int)floorX;
            int cellY = (int)floorY;
            int tx = ((int)(TEX_WIDTH * (floorX - cellX))) & (TEX_WIDTH - 1);
            int ty = ((int)(TEX_HEIGHT * (floorY - cellY))) & (TEX_HEIGHT - 1);
            int texIndex = TEX_WIDTH * ty + tx;
            if (validate_texture(&floor_texture) && validate_texture(&ceiling_texture)) {
                gfx->pixels[y * SCREEN_WIDTH + x] = floor_texture.pixels[texIndex];
                gfx->pixels[(SCREEN_HEIGHT - y - 1) * SCREEN_WIDTH + x] = ceiling_texture.pixels[texIndex];
            }
            floorX += floorStepX;
            floorY += floorStepY;
        }
    }

    // Wall casting
    #pragma omp parallel for schedule(dynamic, 16)
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
        float perpWallDist = (side == 0) ?
            (mapX - player->x + (1 - stepX) / 2.0f) / rayDirX :
            (mapY - player->y + (1 - stepY) / 2.0f) / rayDirY;
        int lineHeight = (int)(SCREEN_HEIGHT / perpWallDist);
        int drawStart = -lineHeight / 2 + SCREEN_HEIGHT / 2;
        if (drawStart < 0) drawStart = 0;
        int drawEnd = lineHeight / 2 + SCREEN_HEIGHT / 2;
        if (drawEnd >= SCREEN_HEIGHT) drawEnd = SCREEN_HEIGHT - 1;
        int tex_id = map[mapY][mapX] - 1;
        if (tex_id < 0 || tex_id >= NUM_WALL_TEXTURES) continue; // skip invalid
        if (validate_texture(&wall_textures[tex_id])) {
            float wallX;
            if (side == 0) wallX = player->y + perpWallDist * rayDirY;
            else wallX = player->x + perpWallDist * rayDirX;
            wallX -= floorf(wallX);
            int texX = (int)(wallX * TEX_WIDTH);
            if ((side == 0 && rayDirX > 0) || (side == 1 && rayDirY < 0))
                texX = TEX_WIDTH - texX - 1;
            float step = (float)TEX_HEIGHT / lineHeight;
            float texPos = (drawStart - SCREEN_HEIGHT / 2 + lineHeight / 2) * step;
            for(int y = drawStart; y < drawEnd; y++) {
                int texY = (int)texPos & (TEX_HEIGHT - 1);
                texPos += step;
                int texIndex = texY * TEX_WIDTH + texX;
                if (texIndex >= 0 && texIndex < TEX_WIDTH * TEX_HEIGHT) {
                    Uint32 color = wall_textures[tex_id].pixels[texIndex];
                    if(side == 1) {
                        Uint8 r = ((color >> 16) & 0xFF) * 0.7;
                        Uint8 g = ((color >> 8) & 0xFF) * 0.7;
                        Uint8 b = (color & 0xFF) * 0.7;
                        color = (0xFF << 24) | (r << 16) | (g << 8) | b;
                    }
                    gfx->pixels[y * SCREEN_WIDTH + x] = color;
                }
            }
        }
    }

    // Update texture with rendered frame
    SDL_UpdateTexture(gfx->texture, NULL, gfx->pixels, SCREEN_WIDTH * sizeof(Uint32));
    SDL_RenderClear(gfx->renderer);
    SDL_RenderCopy(gfx->renderer, gfx->texture, NULL, NULL);
    SDL_RenderPresent(gfx->renderer);
}

void handle_input(Player *player, const Uint8 *keystate, int map[MAP_HEIGHT][MAP_WIDTH]) {
    float newX, newY;
    const float COLLISION_BUFFER = 0.2f;
    // Forward
    if (keystate[SDL_SCANCODE_UP]) {
        newX = player->x + player->dirX * MOVE_SPEED;
        newY = player->y + player->dirY * MOVE_SPEED;
        // X axis
        if (map[(int)player->y][(int)(newX + (player->dirX > 0 ? COLLISION_BUFFER : -COLLISION_BUFFER))] == 0)
            player->x = newX;
        // Y axis
        if (map[(int)(newY + (player->dirY > 0 ? COLLISION_BUFFER : -COLLISION_BUFFER))][(int)player->x] == 0)
            player->y = newY;
    }
    // Backward
    if (keystate[SDL_SCANCODE_DOWN]) {
        newX = player->x - player->dirX * MOVE_SPEED;
        newY = player->y - player->dirY * MOVE_SPEED;
        if (map[(int)player->y][(int)(newX + (player->dirX < 0 ? COLLISION_BUFFER : -COLLISION_BUFFER))] == 0)
            player->x = newX;
        if (map[(int)(newY + (player->dirY < 0 ? COLLISION_BUFFER : -COLLISION_BUFFER))][(int)player->x] == 0)
            player->y = newY;
    }
    // Strafe left (A)
    if (keystate[SDL_SCANCODE_A]) {
        newX = player->x - player->planeX * MOVE_SPEED;
        newY = player->y - player->planeY * MOVE_SPEED;
        if (map[(int)player->y][(int)(newX + (player->planeX < 0 ? COLLISION_BUFFER : -COLLISION_BUFFER))] == 0)
            player->x = newX;
        if (map[(int)(newY + (player->planeY < 0 ? COLLISION_BUFFER : -COLLISION_BUFFER))][(int)player->x] == 0)
            player->y = newY;
    }
    // Strafe right (D)
    if (keystate[SDL_SCANCODE_D]) {
        newX = player->x + player->planeX * MOVE_SPEED;
        newY = player->y + player->planeY * MOVE_SPEED;
        if (map[(int)player->y][(int)(newX + (player->planeX > 0 ? COLLISION_BUFFER : -COLLISION_BUFFER))] == 0)
            player->x = newX;
        if (map[(int)(newY + (player->planeY > 0 ? COLLISION_BUFFER : -COLLISION_BUFFER))][(int)player->x] == 0)
            player->y = newY;
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

