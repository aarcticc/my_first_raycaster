#ifndef TEXTURE_H
#define TEXTURE_H

#include <SDL2/SDL.h>

#define TEXTURE_SIZE 128
#define NUM_TEXTURES 4

void init_textures(SDL_Renderer* renderer);
void destroy_textures();
SDL_Texture* get_texture(int index);

#endif