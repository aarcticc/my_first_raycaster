//! Standart C Library
#include <unistd.h>
#include <errno.h>
//! Project Headers
#include "raycaster.h"
#include "texture.h"
#include "log_utils.h"
#include "start_wrapper.h"
#include "map.h"
//! OPENMP Header
#ifdef _OPENMP
#include <omp.h>
#endif

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
