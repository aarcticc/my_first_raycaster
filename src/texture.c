#include "texture.h"
#include "log_utils.h"
#include <SDL2/SDL_image.h>
#include <string.h>

TextureInfo wall_textures[NUM_TEXTURES] = {0};

int init_textures(SDL_Renderer* renderer) {
    // Initialize SDL_image library for loading PNG files
    int img_flags = IMG_INIT_PNG;
    if (!(IMG_Init(img_flags) & img_flags)) {
        char error_msg[128];
        snprintf(error_msg, sizeof(error_msg), 
                "[SDL_image] Initialization failed: %s", IMG_GetError());
        log_error(log_file, error_msg);
        return 1;
    }

    // Define paths to our texture files
    const char* paths[NUM_TEXTURES] = {
        "assets/wall1.png", "assets/wall2.png",
        "assets/wall3.png", "assets/wall4.png"
    };

    // Load each texture file
    for (int i = 0; i < NUM_TEXTURES; ++i) {
        SDL_Surface* surface = IMG_Load(paths[i]);
        if (!surface) {
            char error_msg[128];
            snprintf(error_msg, sizeof(error_msg), 
                    "[Texture] Failed to load surface %s: %s", paths[i], IMG_GetError());
            log_error(log_file, error_msg);
            destroy_textures();
            IMG_Quit();
            return 1;
        }

        // Convert surface to correct format
        SDL_Surface* formatted = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_ARGB8888, 0);
        SDL_FreeSurface(surface);
        
        if (!formatted) {
            char error_msg[128];
            snprintf(error_msg, sizeof(error_msg), 
                    "[Texture] Failed to format surface %s: %s", paths[i], SDL_GetError());
            log_error(log_file, error_msg);
            destroy_textures();
            IMG_Quit();
            return 1;
        }

        // Create texture from formatted surface
        wall_textures[i].texture = SDL_CreateTextureFromSurface(renderer, formatted);
        wall_textures[i].width = formatted->w;
        wall_textures[i].height = formatted->h;
        
        // Allocate and copy pixel data
        wall_textures[i].pixels = malloc(formatted->w * formatted->h * sizeof(Uint32));
        if (wall_textures[i].pixels) {
            memcpy(wall_textures[i].pixels, formatted->pixels, 
                   formatted->w * formatted->h * sizeof(Uint32));
        }
        
        SDL_FreeSurface(formatted);
        
        if (!wall_textures[i].texture || !wall_textures[i].pixels) {
            char error_msg[128];
            snprintf(error_msg, sizeof(error_msg), 
                    "[Texture] Failed to create texture %s", paths[i]);
            log_error(log_file, error_msg);
            destroy_textures();
            IMG_Quit();
            return 1;
        }

        // Log successful texture load
        char success_msg[128];
        snprintf(success_msg, sizeof(success_msg), 
                "[Texture] Successfully loaded %s", paths[i]);
        log_error(log_file, success_msg);
    }

    return 0;
}

void destroy_textures(void) {
    // Free each texture and clear the pointer
    for (int i = 0; i < NUM_TEXTURES; i++) {
        if (wall_textures[i].texture) {
            SDL_DestroyTexture(wall_textures[i].texture);
            wall_textures[i].texture = NULL;
        }
        if (wall_textures[i].pixels) {
            free(wall_textures[i].pixels);
            wall_textures[i].pixels = NULL;
        }
    }
    // Shutdown SDL_image library
    IMG_Quit();
}

