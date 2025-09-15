//! Standart C Library
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
//! Project Headers
#include "raycaster.h"
#include "texture.h"
#include "log_utils.h"
#include "start_wrapper.h"
#include "map.h"
//! OPENMP Header
#include <omp.h>

int main(void) {
    // Initialize log file name
    get_log_filename(log_file, sizeof(log_file));
    log_separator(log_file, "GAME INITIALIZATION");
    
    // Check if maps directory exists
    struct stat st = {0};
    if (stat("maps", &st) == -1) {
        mkdir("maps", 0700);
        log_error(log_file, "[System] Created maps directory");
    }

    // Try to load map files in order of preference
    const char* map_files[] = {
        "maps/custom_map.json",
        "maps/latest_map.json",
        "maps/default.json"
    };
    
    int map_loaded = 0;
    for (size_t i = 0; i < sizeof(map_files) / sizeof(map_files[0]); i++) {
        if (access(map_files[i], F_OK) != -1) {
            if (load_custom_map(map_files[i]) == 0) {
                char msg[128];
                snprintf(msg, sizeof(msg), "[Map] Successfully loaded %s", map_files[i]);
                log_error(log_file, msg);
                map_loaded = 1;
                break;
            }
        }
    }

    if (!map_loaded) {
        log_error(log_file, "[Map] Using default built-in map");
    }

    // Log OpenMP status
    #ifdef _OPENMP
        log_error(log_file, "[System] OpenMP enabled");
        char omp_msg[64];
        snprintf(omp_msg, sizeof(omp_msg), "[System] Number of available threads: %d", omp_get_max_threads());
        log_error(log_file, omp_msg);
    #else
        log_error(log_file, "[System] OpenMP disabled");
    #endif

    // Initialize SDL library (required for graphics)
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        log_separator(log_file, "ERROR");
        log_error(log_file, "[SDL] Initialization failed");
        return 1;
    }
    log_error(log_file, "[SDL] Successfully initialized");

    // Initialize graphics system (window, renderer, etc.)
    Graphics gfx;
    if (init_graphics(&gfx) != 0) {
        fprintf(stderr, "Graphics init failed.\n");
        log_error(log_file, "[Graphics] Initialization failed");
        return 1;
    }
    log_error(log_file, "[Graphics] Successfully initialized");

    // Load wall textures from files
    if (init_textures(gfx.renderer) != 0) {
        fprintf(stderr, "Texture init failed.\n");
        log_error(log_file, "[Textures] Initialization failed");
        shutdown_graphics(&gfx);
        SDL_Quit();
        return 1;
    }
    log_error(log_file, "[Textures] Successfully loaded all textures");

    // Log successful startup
    log_error(log_file, "[System] All systems initialized successfully");

    // Initialize player position and direction with safety check
    Player player = {
        .x = 1.5f,
        .y = 1.5f,
        .dirX = -1.0f,
        .dirY = 0.0f,
        .planeX = 0.0f,
        .planeY = 0.66f,
        .perpWallDist = {0}  // Initialize all elements to 0
    };
    
    // Check if spawn position is valid, if not find nearest safe spot
    if (!is_valid_position(player.x, player.y)) {
        float newX = player.x;
        float newY = player.y;
        find_nearest_empty_space(&newX, &newY);
        player.x = newX;
        player.y = newY;
        
        char spawn_msg[128];
        snprintf(spawn_msg, sizeof(spawn_msg), 
                "[Player] Unsafe spawn detected, teleported to (%.2f, %.2f)", 
                player.x, player.y);
        log_error(log_file, spawn_msg);
    }

    // Add texture validation logging
    log_error(log_file, "[System] Starting texture validation");
    int valid_textures = 1;
    for(int i = 0; i < NUM_WALL_TEXTURES; i++) {
        if (!validate_texture(&wall_textures[i])) {
            char error_msg[128];
            snprintf(error_msg, sizeof(error_msg), 
                    "[Texture] Wall texture %d validation failed", i + 1);
            log_error(log_file, error_msg);
            valid_textures = 0;
        }
    }
    
    if (!validate_texture(&floor_texture)) {
        log_error(log_file, "[Texture] Floor texture validation failed");
        valid_textures = 0;
    }
    
    if (!validate_texture(&ceiling_texture)) {
        log_error(log_file, "[Texture] Ceiling texture validation failed");
        valid_textures = 0;
    }
    
    if (valid_textures) {
        log_error(log_file, "[System] All textures validated successfully");
    } else {
        log_error(log_file, "[System] Texture validation failed");
        shutdown_graphics(&gfx);
        SDL_Quit();
        return 1;
    }

    // Setup event handling and keyboard state
    SDL_Event event;
    const Uint8 *keystate = SDL_GetKeyboardState(NULL);
    int running = 1;

    log_error(log_file, "[Game] Starting main loop");
    log_separator(log_file, "STARTING MAIN LOOP");
    // Main game loop
    while (running) {
        // Handle SDL events (like window closing)
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                log_error(log_file, "[Game] Received quit signal");
                running = 0;
            }
        }
        
        // Process player input and update position
        handle_input(&player, keystate, map);
        // Render the current frame
        render_frame(&gfx, &player, map);
        
        // Add a small delay to control frame rate (approx. 60 FPS)
        SDL_Delay(16);
    }

    // Cleanup everything in reverse order of initialization
    log_error(log_file, "[System] Starting cleanup");
    log_separator(log_file, "SHUTDOWN SEQUENCE");
    destroy_textures();
    log_error(log_file, "[Textures] Destroyed");
    shutdown_graphics(&gfx);
    log_error(log_file, "[Graphics] Shutdown complete");
    SDL_Quit();
    log_error(log_file, "[SDL] Shutdown complete");
    log_error(log_file, "[System] Clean exit");
    log_separator(log_file, "CLEAN EXIT");
    return 0;
}

// Initialize graphics system - creates window, renderer, and texture buffer
int init_graphics(Graphics *gfx) {
    if (!gfx) return -1;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return -1;
    }

    // Optional: prefer a good scaling backend
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

    gfx->window = SDL_CreateWindow(
        "Raycaster",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    if (!gfx->window) {
        fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    gfx->renderer = SDL_CreateRenderer(gfx->window, -1,
                                       SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!gfx->renderer) {
        fprintf(stderr, "SDL_CreateRenderer failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(gfx->window);
        SDL_Quit();
        return -1;
    }

    gfx->texture = SDL_CreateTexture(gfx->renderer,
                                     SDL_PIXELFORMAT_ARGB8888,
                                     SDL_TEXTUREACCESS_STREAMING,
                                     SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!gfx->texture) {
        fprintf(stderr, "SDL_CreateTexture failed: %s\n", SDL_GetError());
        SDL_DestroyRenderer(gfx->renderer);
        SDL_DestroyWindow(gfx->window);
        SDL_Quit();
        return -1;
    }

    gfx->pixels = malloc(sizeof(Uint32) * SCREEN_WIDTH * SCREEN_HEIGHT);
    if (!gfx->pixels) {
        fprintf(stderr, "malloc for pixels failed\n");
        SDL_DestroyTexture(gfx->texture);
        SDL_DestroyRenderer(gfx->renderer);
        SDL_DestroyWindow(gfx->window);
        SDL_Quit();
        return -1;
    }
    memset(gfx->pixels, 0, sizeof(Uint32) * SCREEN_WIDTH * SCREEN_HEIGHT);

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
    // Pre-calculate values used in floor/ceiling casting
    const float posZ = SCREEN_HEIGHT / 2.0f;
    const float rayDirX0 = player->dirX - player->planeX;
    const float rayDirY0 = player->dirY - player->planeY;
    const float rayDirX1 = player->dirX + player->planeX;
    const float rayDirY1 = player->dirY + player->planeY;

    // FLOOR AND CEILING CASTING
    #pragma omp parallel for schedule(dynamic, 16) shared(gfx, floor_texture, ceiling_texture) \
            firstprivate(posZ, rayDirX0, rayDirY0, rayDirX1, rayDirY1, player)
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
    #pragma omp parallel for schedule(dynamic, 16) shared(gfx, wall_textures, map) \
            firstprivate(player)
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

    // Update texture with rendered frame
    SDL_UpdateTexture(gfx->texture, NULL, gfx->pixels, SCREEN_WIDTH * sizeof(Uint32));
    SDL_RenderClear(gfx->renderer);
    SDL_RenderCopy(gfx->renderer, gfx->texture, NULL, NULL);
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
