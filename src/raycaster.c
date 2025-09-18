//! Standard C Library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//! Project Headers
#include "raycaster.h"
#include "texture.h"
#include "map.h"
#include "log_utils.h"

// Initialize graphics system - creates window, renderer, and texture buffer
int init_graphics(Graphics *gfx) {
    if (!gfx) {
        log_error(log_file, "[Graphics] Invalid graphics pointer");
        return -1;
    }

    // Initialize SDL with all required subsystems
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS) < 0) {
        log_error(log_file, "[SDL] Initialization failed");
        fprintf(stderr, "SDL Error: %s\n", SDL_GetError());
        return -1;
    }

    // Set render quality and disable compositor bypass
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0");
    SDL_SetHint(SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS, "0");

    // Create window with proper flags
    gfx->window = SDL_CreateWindow(
        "Raycaster",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI
    );

    if (!gfx->window) {
        log_error(log_file, "[SDL] Window creation failed");
        fprintf(stderr, "SDL Error: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    // Create renderer with vsync and accelerated flag
    gfx->renderer = SDL_CreateRenderer(gfx->window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!gfx->renderer) {
        log_error(log_file, "[SDL] Renderer creation failed");
        fprintf(stderr, "SDL Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(gfx->window);
        SDL_Quit();
        return -1;
    }

    // Make sure OpenGL/Direct3D coordinate system is set up correctly
    SDL_RenderSetLogicalSize(gfx->renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_SetRenderDrawBlendMode(gfx->renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(gfx->renderer, 0, 0, 0, 255);

    // Create main rendering texture
    gfx->texture = SDL_CreateTexture(gfx->renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        SCREEN_WIDTH, SCREEN_HEIGHT);

    if (!gfx->texture) {
        log_error(log_file, "[SDL] Texture creation failed");
        fprintf(stderr, "SDL Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(gfx->renderer);
        SDL_DestroyWindow(gfx->window);
        SDL_Quit();
        return -1;
    }

    // Allocate pixel buffer
    gfx->pixels = calloc(SCREEN_WIDTH * SCREEN_HEIGHT, sizeof(Uint32));
    if (!gfx->pixels) {
        log_error(log_file, "[Memory] Failed to allocate pixel buffer");
        SDL_DestroyTexture(gfx->texture);
        SDL_DestroyRenderer(gfx->renderer);
        SDL_DestroyWindow(gfx->window);
        SDL_Quit();
        return -1;
    }

    // Clear initial screen
    SDL_RenderClear(gfx->renderer);
    SDL_RenderPresent(gfx->renderer);
    
    log_error(log_file, "[Graphics] Initialization successful");
    return 0;
}

// Clean up and free all graphics resources
void shutdown_graphics(Graphics *gfx) {
    if (!gfx) return;
    if (gfx->pixels) { free(gfx->pixels); gfx->pixels = NULL; }
    if (gfx->texture) { SDL_DestroyTexture(gfx->texture); gfx->texture = NULL; }
    if (gfx->renderer) { SDL_DestroyRenderer(gfx->renderer); gfx->renderer = NULL; }
    if (gfx->window) { SDL_DestroyWindow(gfx->window); gfx->window = NULL; }
    SDL_Quit();
}

// Main rendering function - renders one frame
void render_frame(Graphics *gfx, Player *player, int map[MAP_HEIGHT][MAP_WIDTH]) {
    if (!gfx || !gfx->renderer || !gfx->texture || !gfx->pixels) {
        log_error(log_file, "[Render] Invalid graphics state");
        return;
    }

    // Clear screen
    SDL_SetRenderDrawColor(gfx->renderer, 0, 0, 0, 255);
    SDL_RenderClear(gfx->renderer);

    // Pre-calculate values used in floor/ceiling casting
    const float posZ = SCREEN_HEIGHT / 2.0f;
    const float rayDirX0 = player->dirX - player->planeX;
    const float rayDirY0 = player->dirY - player->planeY;
    const float rayDirX1 = player->dirX + player->planeX;
    const float rayDirY1 = player->dirY + player->planeY;

    // FLOOR AND CEILING CASTING
    for(int y = SCREEN_HEIGHT/2; y < SCREEN_HEIGHT; y++) {
        const float rowDistance = posZ / (y - SCREEN_HEIGHT/2);
        const float floorStepX = rowDistance * (rayDirX1 - rayDirX0) / SCREEN_WIDTH;
        const float floorStepY = rowDistance * (rayDirY1 - rayDirY0) / SCREEN_WIDTH;
        
        float floorX = player->x + rowDistance * rayDirX0;
        float floorY = player->y + rowDistance * rayDirY0;

        for(int x = 0; x < SCREEN_WIDTH; ++x) {
            const int cellX = (int)floorX;
            const int cellY = (int)floorY;
            const int tx = ((int)(TEX_WIDTH * (floorX - cellX))) & (TEX_WIDTH - 1);
            const int ty = ((int)(TEX_HEIGHT * (floorY - cellY))) & (TEX_HEIGHT - 1);
            const int texIndex = TEX_WIDTH * ty + tx;

            if (validate_texture(&floor_texture) && validate_texture(&ceiling_texture)) {
                gfx->pixels[y * SCREEN_WIDTH + x] = floor_texture.pixels[texIndex];
                gfx->pixels[(SCREEN_HEIGHT - y - 1) * SCREEN_WIDTH + x] = ceiling_texture.pixels[texIndex];
            }

            floorX += floorStepX;
            floorY += floorStepY;
        }
    }

    // WALL CASTING
    for (int x = 0; x < SCREEN_WIDTH; x++) {
        // Calculate ray position and direction
        float cameraX = 2.0f * x / SCREEN_WIDTH - 1.0f;  // x-coordinate in camera space
        float rayDirX = player->dirX + player->planeX * cameraX;  // Ray direction X
        float rayDirY = player->dirY + player->planeY * cameraX;  // Ray direction Y

        // Which box of the map we're in
        int mapX = (int)player->x;
        int mapY = (int)player->y;

        // Length of ray from current position to next x or y-side
        // Avoid division by zero with 1e30 (very large number)
        float deltaDistX = (rayDirX == 0) ? 1e30 : fabsf(1 / rayDirX);
        float deltaDistY = (rayDirY == 0) ? 1e30 : fabsf(1 / rayDirY);
        float sideDistX, sideDistY;

        // Calculate step and initial sideDist
        int stepX = (rayDirX < 0) ? -1 : 1;  // Direction to step in x
        int stepY = (rayDirY < 0) ? -1 : 1;  // Direction to step in y
        
        // Calculate distance to first x and y intersections
        sideDistX = (rayDirX < 0) ? 
            (player->x - mapX) * deltaDistX : (mapX + 1.0f - player->x) * deltaDistX;
        sideDistY = (rayDirY < 0) ? 
            (player->y - mapY) * deltaDistY : (mapY + 1.0f - player->y) * deltaDistY;

        // Perform DDA (Digital Differential Analysis)
        int hit = 0, side;  // hit = wall hit? side = which side was hit?
        while (!hit) {
            // Jump to next map square in x or y direction
            if (sideDistX < sideDistY) {
                sideDistX += deltaDistX;
                mapX += stepX;
                side = 0;  // X side was hit
            } else {
                sideDistY += deltaDistY;
                mapY += stepY;
                side = 1;  // Y side was hit
            }
            if (map[mapY][mapX] > 0) hit = 1;  // Check if ray hit a wall
        }

        // Calculate distance to the wall that was hit
        float perpWallDist = (side == 0) ?
            (mapX - player->x + (1 - stepX) / 2.0f) / rayDirX :
            (mapY - player->y + (1 - stepY) / 2.0f) / rayDirY;
            
        // Store the perpendicular wall distance for sprite rendering
        player->perpWallDist[x] = perpWallDist;

        // Calculate height of line to draw on screen
        int lineHeight = (int)(SCREEN_HEIGHT / perpWallDist);
        if (lineHeight <= 0) lineHeight = 1; // Prevent division by zero

        // Calculate lowest and highest pixel to fill in current stripe
        int drawStart = -lineHeight / 2 + SCREEN_HEIGHT / 2;
        if (drawStart < 0) drawStart = 0;
        int drawEnd = lineHeight / 2 + SCREEN_HEIGHT / 2;
        if (drawEnd >= SCREEN_HEIGHT) drawEnd = SCREEN_HEIGHT - 1;

        // Calculate texture coordinates
        float wallX;  // Where exactly the wall was hit
        if (side == 0) wallX = player->y + perpWallDist * rayDirY;
        else wallX = player->x + perpWallDist * rayDirX;
        wallX -= floor(wallX);

        // x coordinate on the texture
        int texX = (int)(wallX * (float)TEX_WIDTH);
        if (side == 0 && rayDirX > 0) texX = TEX_WIDTH - texX - 1;
        if (side == 1 && rayDirY < 0) texX = TEX_WIDTH - texX - 1;

        // Get wall texture ID (subtract 1 since walls start at 1 in map)
        int tex_id = map[mapY][mapX] - 1;
        if (tex_id < 0) tex_id = 0;
        if (tex_id >= NUM_WALL_TEXTURES) tex_id = NUM_WALL_TEXTURES - 1;
        
        float step = (float)TEX_HEIGHT / lineHeight;
        float texPos = (drawStart - SCREEN_HEIGHT / 2 + lineHeight / 2) * step;
        
        // Draw the vertical line pixel by pixel
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

    // Update texture with new frame data
    if (SDL_UpdateTexture(gfx->texture, NULL, gfx->pixels, 
                         SCREEN_WIDTH * sizeof(Uint32)) < 0) {
        log_error(log_file, "[Render] Texture update failed");
        return;
    }

    // Copy texture to renderer
    if (SDL_RenderCopy(gfx->renderer, gfx->texture, NULL, NULL) < 0) {
        log_error(log_file, "[Render] Texture copy failed");
        return;
    }

    // Present the frame
    SDL_RenderPresent(gfx->renderer);
}

// Handle player movement and rotation using keyboard input
void handle_input(Player *player, const Uint8 *keystate, int map[MAP_HEIGHT][MAP_WIDTH]) {
    float newX, newY;
    
    // Forward movement
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
