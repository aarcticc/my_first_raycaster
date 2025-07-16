#ifndef ENEMY_H
#define ENEMY_H

#include <SDL2/SDL.h>
#include "texture.h"

#define MAX_ENEMIES 10
#define ENEMY_WIDTH 32
#define ENEMY_HEIGHT 32

typedef struct {
    float x, y;          // Enemy position
    TextureInfo texture; // Enemy texture
    int active;          // Whether enemy is alive
} Enemy;

extern Enemy enemies[MAX_ENEMIES];

// Initialize enemy system and generate textures
int init_enemies(SDL_Renderer* renderer);

// Generate a procedural texture for an enemy
void generate_enemy_texture(SDL_Renderer* renderer, TextureInfo* texture);

// Clean up enemy resources
void destroy_enemies(void);

#endif
