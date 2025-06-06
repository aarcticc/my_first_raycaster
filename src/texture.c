#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define TEXTURE_SIZE 128
#define NUM_TEXTURES 4

SDL_Texture* textures[NUM_TEXTURES] = {NULL};

static void generate_checkerboard(uint32_t* pixels) {
    for (int y = 0; y < TEXTURE_SIZE; ++y) {
        for (int x = 0; x < TEXTURE_SIZE; ++x) {
            int check = ((x / 16) % 2) ^ ((y / 16) % 2);
            pixels[y * TEXTURE_SIZE + x] = check ? 0xFFAAAAAA : 0xFF444444;
        }
    }
}

static void generate_vertical_stripes(uint32_t* pixels) {
    for (int y = 0; y < TEXTURE_SIZE; ++y) {
        for (int x = 0; x < TEXTURE_SIZE; ++x) {
            int stripe = (x / 8) % 2;
            pixels[y * TEXTURE_SIZE + x] = stripe ? 0xFF2222CC : 0xFF111133;
        }
    }
}

static void generate_horizontal_lines(uint32_t* pixels) {
    for (int y = 0; y < TEXTURE_SIZE; ++y) {
        for (int x = 0; x < TEXTURE_SIZE; ++x) {
            int line = (y / 8) % 2;
            pixels[y * TEXTURE_SIZE + x] = line ? 0xFF00AA00 : 0xFF003300;
        }
    }
}

static void generate_noise(uint32_t* pixels) {
    for (int i = 0; i < TEXTURE_SIZE * TEXTURE_SIZE; ++i) {
        uint8_t val = rand() % 256;
        pixels[i] = 0xFF000000 | (val << 16) | (val << 8) | val;
    }
}

void init_textures(SDL_Renderer* renderer) {
    for (int i = 0; i < NUM_TEXTURES; ++i) {
        uint32_t* pixels = malloc(sizeof(uint32_t) * TEXTURE_SIZE * TEXTURE_SIZE);
        if (!pixels) continue;

        switch (i) {
            case 0: generate_checkerboard(pixels); break;
            case 1: generate_vertical_stripes(pixels); break;
            case 2: generate_horizontal_lines(pixels); break;
            case 3: generate_noise(pixels); break;
            default: memset(pixels, 0, TEXTURE_SIZE * TEXTURE_SIZE * sizeof(uint32_t)); break;
        }

        SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(
            pixels, TEXTURE_SIZE, TEXTURE_SIZE, 32, TEXTURE_SIZE * 4,
            0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000
        );

        if (!surface) {
            free(pixels);
            continue;
        }

        textures[i] = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        free(pixels);
    }
}

void destroy_textures() {
    for (int i = 0; i < NUM_TEXTURES; ++i) {
        if (textures[i]) {
            SDL_DestroyTexture(textures[i]);
            textures[i] = NULL;
        }
    }
}

SDL_Texture* get_texture(int index) {
    if (index < 0 || index >= NUM_TEXTURES) return NULL;
    return textures[index];
}