#include "raycaster.h"
#include "texture.h"
#include "log_utils.h"
#include "enemy.h"
#ifdef _OPENMP
#include <omp.h>
#endif
#include <sys/stat.h>
#include <unistd.h>

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
    
    // Check if mapmaker is available
    if (access("tools/mapmaker.py", X_OK) == -1) {
        log_error(log_file, "[System] Warning: Mapmaker tool not found or not executable");
    } else {
        log_error(log_file, "[System] Mapmaker tool available");
    }

    // Try to load custom map
    if (load_custom_map("maps/custom_map.json") != 0) {
        log_error(log_file, "[Map] Using default map due to loading failure");
    } else {
        log_error(log_file, "[Map] Custom map loaded successfully");
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

    // Initialize enemies after textures are loaded
    if (init_enemies(gfx.renderer) != 0) {
        log_error(log_file, "[Enemies] Failed to initialize");
        destroy_enemies();
        destroy_textures();
        shutdown_graphics(&gfx);
        SDL_Quit();
        return 1;
    }
    log_error(log_file, "[Enemies] Successfully initialized");

    // Log successful startup
    log_error(log_file, "[System] All systems initialized successfully");

    // Initialize player position and direction with safety check
    Player player = {1.5f, 1.5f, -1.0f, 0.0f, 0.0f, 0.66f};
    
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

    // Spawn some enemies at predetermined positions
    spawn_enemy_at(2.5f, 2.5f);
    spawn_enemy_at(18.5f, 18.5f);

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

        // Update enemy positions
        update_enemies(&player, map);
        
        // Process player input and update position
        handle_input(&player, keystate, map);
        // Render the current frame
        render_frame(&gfx, &player, map);
        // Render enemies
        render_enemies(&gfx, &player);
        
        // Check if player is caught and render indicator
        if (is_player_caught(&player)) {
            render_caught_indicator(&gfx);
        }
        
        // Add a small delay to control frame rate (approx. 60 FPS)
        SDL_Delay(16);
    }

    // Cleanup everything in reverse order of initialization
    log_error(log_file, "[System] Starting cleanup");
    log_separator(log_file, "SHUTDOWN SEQUENCE");
    destroy_enemies();
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
