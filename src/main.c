#include "raycaster.h"
#include "map.h"
#include <stdio.h>

int main(void) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }

    Graphics gfx;
    if (init_graphics(&gfx) != 0) {
        fprintf(stderr, "Graphics init failed.\n");
        return 1;
    }

    Player player = {3.5f, 3.5f, -1.0f, 0.0f, 0.0f, 0.66f};
    SDL_Event event;
    const Uint8 *keystate = SDL_GetKeyboardState(NULL);
    int running = 1;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;
        }

        handle_input(&player, keystate, map);
        render_frame(&gfx, &player, map);
        SDL_Delay(16);
    }

    shutdown_graphics(&gfx);
    return 0;
}