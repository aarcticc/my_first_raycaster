#include "raycaster.h"
#include "texture.h"
#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif

int main(void) {
    // Initialize SDL library (required for graphics)
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }

    // Initialize graphics system (window, renderer, etc.)
    Graphics gfx;
    if (init_graphics(&gfx) != 0) {
        fprintf(stderr, "Graphics init failed.\n");
        return 1;
    }

    // Load wall textures from files
    if (init_textures(gfx.renderer) != 0) {
        fprintf(stderr, "Texture init failed.\n");
        shutdown_graphics(&gfx);
        SDL_Quit();
        return 1;
    }

    // Load map before player initialization
    if (!load_map("maps/default.map", map)) {
        fprintf(stderr, "Warning: Could not load map. Initializing default map.\n");
        // Default: border walls, empty inside
        for (int y = 0; y < MAP_HEIGHT; ++y)
            for (int x = 0; x < MAP_WIDTH; ++x)
                map[y][x] = (y == 0 || y == MAP_HEIGHT-1 || x == 0 || x == MAP_WIDTH-1) ? 1 : 0;
    }

    // Initialize player position and direction with safety check
    Player player = {1.5f, 1.5f, -1.0f, 0.0f, 0.0f, 0.66f};
    
    // Check if spawn position is valid, if not find nearest safe spot
    if (!is_valid_position(player.x, player.y)) {
        float newX = player.x;
        float newY = player.y;
        find_nearest_empty_space(&newX, &newY);
        player.x = newX;
        player.y = newY;
    }

    // Texture validation
    int valid_textures = 1;
    for(int i = 0; i < NUM_WALL_TEXTURES; i++) {
        if (!validate_texture(&wall_textures[i])) {
            valid_textures = 0;
        }
    }
    if (!validate_texture(&floor_texture)) valid_textures = 0;
    if (!validate_texture(&ceiling_texture)) valid_textures = 0;
    if (!valid_textures) {
        shutdown_graphics(&gfx);
        SDL_Quit();
        return 1;
    }

    SDL_Event event;
    const Uint8 *keystate = SDL_GetKeyboardState(NULL);
    int running = 1;

    // Main game loop
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        handle_input(&player, keystate, map);
        render_frame(&gfx, &player, map);
        SDL_Delay(16);
    }

    destroy_textures();
    shutdown_graphics(&gfx);
    SDL_Quit();
    return 0;
}
