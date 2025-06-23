#include "texture.h"
#include "log_utils.h"
#include <SDL2/SDL_image.h>
#include <string.h>
#include <stdlib.h>

TextureInfo wall_textures[NUM_WALL_TEXTURES] = {0};
TextureInfo floor_texture = {0};
TextureInfo ceiling_texture = {0};

int validate_texture(const TextureInfo* tex) {
    return tex && tex->pixels && tex->width == TEX_WIDTH && tex->height == TEX_HEIGHT;
}

static SDL_Surface* load_and_format_texture(const char* path) {
    SDL_Surface* surface = IMG_Load(path);
    if (!surface) return NULL;
    SDL_Surface* formatted = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_ARGB8888, 0);
    SDL_FreeSurface(surface);
    return formatted;
}

int init_textures(SDL_Renderer* renderer) {
    int img_flags = IMG_INIT_PNG;
    if (!(IMG_Init(img_flags) & img_flags)) {
        char error_msg[128];
        snprintf(error_msg, sizeof(error_msg), "[SDL_image] Initialization failed: %s", IMG_GetError());
        log_error(log_file, error_msg);
        return 1;
    }

    const char* wall_paths[NUM_WALL_TEXTURES] = {
        "assets/wall1.png", "assets/wall2.png",
        "assets/wall3.png", "assets/wall4.png"
    };

    for (int i = 0; i < NUM_WALL_TEXTURES; ++i) {
        SDL_Surface* formatted = load_and_format_texture(wall_paths[i]);
        if (!formatted || formatted->w != TEX_WIDTH || formatted->h != TEX_HEIGHT) {
            log_error(log_file, "[Texture] Invalid wall texture dimensions or load failure");
            if (formatted) SDL_FreeSurface(formatted);
            destroy_textures();
            return 1;
        }
        wall_textures[i].texture = SDL_CreateTextureFromSurface(renderer, formatted);
        wall_textures[i].width = formatted->w;
        wall_textures[i].height = formatted->h;
        wall_textures[i].pixels = malloc(formatted->w * formatted->h * sizeof(Uint32));
        if (wall_textures[i].pixels) {
            memcpy(wall_textures[i].pixels, formatted->pixels, formatted->w * formatted->h * sizeof(Uint32));
        }
        SDL_FreeSurface(formatted);
        if (!wall_textures[i].texture || !wall_textures[i].pixels) {
            log_error(log_file, "[Texture] Failed to create wall texture");
            destroy_textures();
            return 1;
        }
    }

    SDL_Surface* floor_formatted = load_and_format_texture("assets/floor.png");
    SDL_Surface* ceiling_formatted = load_and_format_texture("assets/ceiling.png");

    if (!floor_formatted || !ceiling_formatted ||
        floor_formatted->w != TEX_WIDTH || floor_formatted->h != TEX_HEIGHT ||
        ceiling_formatted->w != TEX_WIDTH || ceiling_formatted->h != TEX_HEIGHT) {
        log_error(log_file, "[Texture] Invalid floor/ceiling texture dimensions or load failure");
        if (floor_formatted) SDL_FreeSurface(floor_formatted);
        if (ceiling_formatted) SDL_FreeSurface(ceiling_formatted);
        destroy_textures();
        return 1;
    }

    floor_texture.texture = SDL_CreateTextureFromSurface(renderer, floor_formatted);
    floor_texture.width = floor_formatted->w;
    floor_texture.height = floor_formatted->h;
    floor_texture.pixels = malloc(floor_formatted->w * floor_formatted->h * sizeof(Uint32));
    if (floor_texture.pixels) {
        memcpy(floor_texture.pixels, floor_formatted->pixels, floor_formatted->w * floor_formatted->h * sizeof(Uint32));
    }
    SDL_FreeSurface(floor_formatted);

    ceiling_texture.texture = SDL_CreateTextureFromSurface(renderer, ceiling_formatted);
    ceiling_texture.width = ceiling_formatted->w;
    ceiling_texture.height = ceiling_formatted->h;
    ceiling_texture.pixels = malloc(ceiling_formatted->w * ceiling_formatted->h * sizeof(Uint32));
    if (ceiling_texture.pixels) {
        memcpy(ceiling_texture.pixels, ceiling_formatted->pixels, ceiling_formatted->w * ceiling_formatted->h * sizeof(Uint32));
    }
    SDL_FreeSurface(ceiling_formatted);

    return 0;
}

void destroy_textures(void) {
    for (int i = 0; i < NUM_WALL_TEXTURES; i++) {
        if (wall_textures[i].texture) {
            SDL_DestroyTexture(wall_textures[i].texture);
            wall_textures[i].texture = NULL;
        }
        if (wall_textures[i].pixels) {
            free(wall_textures[i].pixels);
            wall_textures[i].pixels = NULL;
        }
    }
    if (floor_texture.texture) {
        SDL_DestroyTexture(floor_texture.texture);
        floor_texture.texture = NULL;
    }
    if (floor_texture.pixels) {
        free(floor_texture.pixels);
        floor_texture.pixels = NULL;
    }
    if (ceiling_texture.texture) {
        SDL_DestroyTexture(ceiling_texture.texture);
        ceiling_texture.texture = NULL;
    }
    if (ceiling_texture.pixels) {
        free(ceiling_texture.pixels);
        ceiling_texture.pixels = NULL;
    }
    IMG_Quit();
}
            SDL_DestroyTexture(wall_textures[i].texture);
            wall_textures[i].texture = NULL;
        }
        if (wall_textures[i].pixels) {
            free(wall_textures[i].pixels);
            wall_textures[i].pixels = NULL;
        }
    }

    // Free floor texture
    if (floor_texture.texture) {
        SDL_DestroyTexture(floor_texture.texture);
        floor_texture.texture = NULL;
    }
    if (floor_texture.pixels) {
        free(floor_texture.pixels);
        floor_texture.pixels = NULL;
    }

    // Free ceiling texture
    if (ceiling_texture.texture) {
        SDL_DestroyTexture(ceiling_texture.texture);
        ceiling_texture.texture = NULL;
    }
    if (ceiling_texture.pixels) {
        free(ceiling_texture.pixels);
        ceiling_texture.pixels = NULL;
    }

    IMG_Quit();
}

