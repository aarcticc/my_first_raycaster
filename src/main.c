#include "raycaster.h"
#include "texture.h"
#include "map.h" // remove if not needed
#include <stdio.h>

// main function with sdl, gfx init error messages; player; running loop
int main(void) {
    // SDL error message
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }

    // graphics (gfx) error message
    Graphics gfx;
    if (init_graphics(&gfx) != 0) {
        fprintf(stderr, "Graphics init failed.\n");
        SDL_Quit();
        return 1;
    }

    // initialize textures
    if (init_textures(gfx.renderer) != 0) {
        fprintf(stderr, "Texture init failed.\n");
        shutdown_graphics(&gfx);
        SDL_Quit();
        return 1;
    }

    // player
    Player player = {1.5f, 1.5f, -1.0f, 0.0f, 0.0f, 0.66f}; // x, y, direction, etc.
    SDL_Event event;
    const Uint8 *keystate = SDL_GetKeyboardState(NULL);
    int running = 1;

    // main loop
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;
        }

        handle_input(&player, keystate, map);
        render_frame(&gfx, &player, map);
        SDL_Delay(16); // 60 FPS cap
    }

    // cleanup
    destroy_textures();
    shutdown_graphics(&gfx);
    SDL_Quit();
    return 0;
}