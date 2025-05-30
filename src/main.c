#include "raycaster.h"
#include "map.h"
#include <stdio.h>

// main function with sdl,gfx init error messages; player; running loop
int main(void) {
    // SDL error message
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }

    // gxf error message
    Graphics gfx;
    if (init_graphics(&gfx) != 0) {
        fprintf(stderr, "Graphics init failed.\n");
        return 1;
    }

    // player
    Player player = {1.5f, 1.5f, -1.0f, 0.0f, 0.0f, 0.66f};     // starting attributes {x(spawn),y(spawn),z(spawn),velocity, direction, fov}
    SDL_Event event;
    const Uint8 *keystate = SDL_GetKeyboardState(NULL);     //sets input to none
    int running = 1;

    // player running loop
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;
        }

        handle_input(&player, keystate, map);     //input handling
        render_frame(&gfx, &player, map);
        SDL_Delay(16);     //60fps limit
    }

    shutdown_graphics(&gfx);
    return 0;
}