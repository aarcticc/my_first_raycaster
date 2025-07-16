#include "enemy.h"
#include "log_utils.h"
#include <stdlib.h>
#include <math.h>

Enemy enemies[MAX_ENEMIES] = {0};

int init_enemies(SDL_Renderer* renderer) {
    log_error(log_file, "[Enemies] Initializing enemy system");
    
    for (int i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].active = 0;
        enemies[i].x = 0;
        enemies[i].y = 0;
        
        // Generate unique texture for each enemy
        if (create_empty_texture(renderer, &enemies[i].texture, ENEMY_WIDTH, ENEMY_HEIGHT) != 0) {
            log_error(log_file, "[Enemies] Failed to create enemy texture");
            return 1;
        }
        generate_enemy_texture(renderer, &enemies[i].texture);
    }
    
    log_error(log_file, "[Enemies] Enemy system initialized successfully");
    return 0;
}

void generate_enemy_texture(SDL_Renderer* renderer, TextureInfo* texture) {
    // Generate a simple geometric pattern
    for (int y = 0; y < ENEMY_HEIGHT; y++) {
        for (int x = 0; x < ENEMY_WIDTH; x++) {
            Uint32 color;
            
            // Create a circle pattern
            float centerX = ENEMY_WIDTH / 2.0f;
            float centerY = ENEMY_HEIGHT / 2.0f;
            float dx = x - centerX;
            float dy = y - centerY;
            float distance = sqrtf(dx * dx + dy * dy);
            
            if (distance < ENEMY_WIDTH / 3) {
                // Core of the enemy
                color = 0xFFFF0000; // Red
            } else if (distance < ENEMY_WIDTH / 2) {
                // Outer ring
                color = 0xFF000000; // Black
            } else {
                // Transparent background
                color = 0x00000000;
            }
            
            texture->pixels[y * ENEMY_WIDTH + x] = color;
        }
    }
    
    update_texture_pixels(texture);
}

void destroy_enemies(void) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].texture.texture) {
            SDL_DestroyTexture(enemies[i].texture.texture);
            free(enemies[i].texture.pixels);
        }
    }
    log_error(log_file, "[Enemies] Enemy system destroyed");
}
