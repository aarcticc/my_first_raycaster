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

void generate_enemy_texture(SDL_Renderer* renderer __attribute__((unused)), TextureInfo* texture) {
    // Generate a pattern with multiple colors
    for (int y = 0; y < ENEMY_HEIGHT; y++) {
        for (int x = 0; x < ENEMY_WIDTH; x++) {
            Uint32 color = 0x00000000; // Transparent by default
            
            // Calculate normalized coordinates from center (-1 to 1)
            float nx = (x - ENEMY_WIDTH/2.0f) / (ENEMY_WIDTH/2.0f);
            float ny = (y - ENEMY_HEIGHT/2.0f) / (ENEMY_HEIGHT/2.0f);
            float dist = sqrtf(nx*nx + ny*ny);
            
            // Create a layered circular pattern
            if (dist < 0.8f) {
                if (dist < 0.3f) {
                    // Core - bright red
                    color = 0xFFFF0000;
                } else if (dist < 0.5f) {
                    // Inner ring - dark red
                    color = 0xFF800000;
                } else if (dist < 0.65f) {
                    // Outer ring - black
                    color = 0xFF000000;
                } else {
                    // Edge fade - semi-transparent black
                    float alpha = (0.8f - dist) / 0.15f;
                    Uint8 a = (Uint8)(alpha * 255);
                    color = (a << 24);
                }
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

void spawn_enemy_at(float x, float y) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) {
            enemies[i].x = x;
            enemies[i].y = y;
            enemies[i].active = 1;
            enemies[i].dirX = 0;
            enemies[i].dirY = 0;
            enemies[i].lastMoveTime = SDL_GetTicks();
            log_error(log_file, "[Enemy] Spawned new enemy");
            break;
        }
    }
}

void update_enemies(const Player* player, int map[MAP_HEIGHT][MAP_WIDTH]) {
    Uint32 currentTime = SDL_GetTicks();
    
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) continue;
        
        if (currentTime - enemies[i].lastMoveTime < ENEMY_MOVE_DELAY) continue;
        
        float dx = player->x - enemies[i].x;
        float dy = player->y - enemies[i].y;
        float length = sqrtf(dx * dx + dy * dy);
        
        if (length > 0) {
            dx /= length;
            dy /= length;
            
            float newX = enemies[i].x + dx * ENEMY_SPEED;
            float newY = enemies[i].y + dy * ENEMY_SPEED;
            
            // Check collision with walls and update position if valid
            if (newY >= 0 && newY < MAP_HEIGHT && map[(int)newY][(int)enemies[i].x] == 0) {
                enemies[i].y = newY;
            }
            if (newX >= 0 && newX < MAP_WIDTH && map[(int)enemies[i].y][(int)newX] == 0) {
                enemies[i].x = newX;
            }
            
            enemies[i].dirX = dx;
            enemies[i].dirY = dy;
        }
        
        enemies[i].lastMoveTime = currentTime;
    }
}

int is_player_caught(const Player* player) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) continue;
        
        float dx = player->x - enemies[i].x;
        float dy = player->y - enemies[i].y;
        float distance = sqrtf(dx * dx + dy * dy);
        
        if (distance < ENEMY_CATCH_DISTANCE) {
            return 1;
        }
    }
    return 0;
}

void render_enemies(Graphics* gfx, const Player* player) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) continue;

        // Calculate enemy position relative to player
        float spriteX = enemies[i].x - player->x;
        float spriteY = enemies[i].y - player->y;

        // Transform sprite with the inverse camera matrix
        float invDet = 1.0f / (player->planeX * player->dirY - player->dirX * player->planeY);
        float transformX = invDet * (player->dirY * spriteX - player->dirX * spriteY);
        float transformY = invDet * (-player->planeY * spriteX + player->planeX * spriteY);

        if (transformY <= 0) continue;

        // Calculate sprite screen position
        int spriteScreenX = (int)((SCREEN_WIDTH / 2) * (1 + transformX / transformY));
        int spriteHeight = abs((int)(SCREEN_HEIGHT / transformY));
        
        // Calculate drawing bounds
        int drawStartY = -spriteHeight / 2 + SCREEN_HEIGHT / 2;
        if (drawStartY < 0) drawStartY = 0;
        int drawEndY = spriteHeight / 2 + SCREEN_HEIGHT / 2;
        if (drawEndY >= SCREEN_HEIGHT) drawEndY = SCREEN_HEIGHT - 1;

        int spriteWidth = abs((int)(SCREEN_HEIGHT / transformY));
        int drawStartX = -spriteWidth / 2 + spriteScreenX;
        if (drawStartX < 0) drawStartX = 0;
        int drawEndX = spriteWidth / 2 + spriteScreenX;
        if (drawEndX >= SCREEN_WIDTH) drawEndX = SCREEN_WIDTH - 1;

        // Render sprite
        for (int stripe = drawStartX; stripe < drawEndX; stripe++) {
            int texX = (int)(256 * (stripe - (-spriteWidth / 2 + spriteScreenX)) * ENEMY_WIDTH / spriteWidth) / 256;
            if (transformY > 0 && stripe > 0 && stripe < SCREEN_WIDTH && transformY < 1000.0) {
                for (int y = drawStartY; y < drawEndY; y++) {
                    int d = (y) * 256 - SCREEN_HEIGHT * 128 + spriteHeight * 128;
                    int texY = ((d * ENEMY_HEIGHT) / spriteHeight) / 256;
                    
                    if (texX >= 0 && texX < ENEMY_WIDTH && texY >= 0 && texY < ENEMY_HEIGHT) {
                        Uint32 color = enemies[i].texture.pixels[ENEMY_WIDTH * texY + texX];
                        if ((color & 0xFF000000) != 0 && 
                            y >= 0 && y < SCREEN_HEIGHT && 
                            stripe >= 0 && stripe < SCREEN_WIDTH) { 
                            gfx->pixels[y * SCREEN_WIDTH + stripe] = color;
                        }
                    }
                }
            }
        }
    }
}

void render_caught_indicator(Graphics* gfx) {
    // Draw red dot in upper right corner
    int dotSize = 10;
    int margin = 20;
    
    for (int y = margin; y < margin + dotSize; y++) {
        for (int x = SCREEN_WIDTH - margin - dotSize; x < SCREEN_WIDTH - margin; x++) {
            if ((x - (SCREEN_WIDTH - margin - dotSize/2)) * (x - (SCREEN_WIDTH - margin - dotSize/2)) +
                (y - (margin + dotSize/2)) * (y - (margin + dotSize/2)) < (dotSize/2) * (dotSize/2)) {
                gfx->pixels[y * SCREEN_WIDTH + x] = 0xFFFF0000;
            }
        }
    }
}