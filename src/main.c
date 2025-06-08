#include "raycaster.h"
#include "texture.h"
#include "log_utils.h"

int main(void) {
    // Initialize log file name
    get_log_filename(log_file, sizeof(log_file));

    // Initialize SDL library (required for graphics)
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
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

    // Initialize player position and direction
    // Parameters: x, y, directionX, directionY, planeX, planeY
    Player player = {1.5f, 1.5f, -1.0f, 0.0f, 0.0f, 0.66f};
    
    // Setup event handling and keyboard state
    SDL_Event event;
    const Uint8 *keystate = SDL_GetKeyboardState(NULL);
    int running = 1;

    log_error(log_file, "[Game] Starting main loop");

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
    destroy_textures();
    log_error(log_file, "[Textures] Destroyed");
    shutdown_graphics(&gfx);
    log_error(log_file, "[Graphics] Shutdown complete");
    SDL_Quit();
    log_error(log_file, "[SDL] Shutdown complete");
    log_error(log_file, "[System] Clean exit");
    return 0;
}
