#include "texture.h"
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
    if (!surface) {
        fprintf(stderr, "IMG_Load failed for '%s': %s\n", path, IMG_GetError());
        return NULL;
    }
    SDL_Surface* formatted = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_ARGB8888, 0);
    if (!formatted) {
        fprintf(stderr, "SDL_ConvertSurfaceFormat failed for '%s': %s\n", path, SDL_GetError());
    }
    SDL_FreeSurface(surface);
    return formatted;
}

int init_textures(SDL_Renderer* renderer) {
    int img_flags = IMG_INIT_PNG;
    if (!(IMG_Init(img_flags) & img_flags)) {
        fprintf(stderr, "IMG_Init failed: %s\n", IMG_GetError());
        return 1;
    }

    const char* wall_paths[NUM_WALL_TEXTURES] = {
        "assets/wall1.png", "assets/wall2.png",
        "assets/wall3.png", "assets/wall4.png"
    };

    for (int i = 0; i < NUM_WALL_TEXTURES; ++i) {
        SDL_Surface* formatted = load_and_format_texture(wall_paths[i]);
        if (!formatted || formatted->w != TEX_WIDTH || formatted->h != TEX_HEIGHT) {
            if (!formatted)
                fprintf(stderr, "Failed to load wall texture %d: %s\n", i+1, wall_paths[i]);
            else
                fprintf(stderr, "Wall texture %d has wrong size: %dx%d (expected %dx%d)\n",
                        i+1, formatted->w, formatted->h, TEX_WIDTH, TEX_HEIGHT);
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
            destroy_textures();
            return 1;
        }
    }

    SDL_Surface* floor_formatted = load_and_format_texture("assets/floor.png");
    SDL_Surface* ceiling_formatted = load_and_format_texture("assets/ceiling.png");

    if (!floor_formatted || !ceiling_formatted ||
        floor_formatted->w != TEX_WIDTH || floor_formatted->h != TEX_HEIGHT ||
        ceiling_formatted->w != TEX_WIDTH || ceiling_formatted->h != TEX_HEIGHT) {
        if (!floor_formatted)
            fprintf(stderr, "Failed to load floor texture: assets/floor.png\n");
        else if (floor_formatted->w != TEX_WIDTH || floor_formatted->h != TEX_HEIGHT)
            fprintf(stderr, "Floor texture has wrong size: %dx%d (expected %dx%d)\n",
                    floor_formatted->w, floor_formatted->h, TEX_WIDTH, TEX_HEIGHT);
        if (!ceiling_formatted)
            fprintf(stderr, "Failed to load ceiling texture: assets/ceiling.png\n");
        else if (ceiling_formatted->w != TEX_WIDTH || ceiling_formatted->h != TEX_HEIGHT)
            fprintf(stderr, "Ceiling texture has wrong size: %dx%d (expected %dx%d)\n",
                    ceiling_formatted->w, ceiling_formatted->h, TEX_WIDTH, TEX_HEIGHT);
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

