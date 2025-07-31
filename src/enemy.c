#include "enemy.h"
#include "log_utils.h"
#include <stdlib.h>
#include <math.h>

Enemy enemies[MAX_ENEMIES] = {
    {1.0f, 9.0f, {0}, ENEMY_GUARD, 25, 1, 0.0f, 0.0f, 0.0f, 0},  // guard at (1, 9)
    {1.0f, 2.0f, {0}, ENEMY_GUARD, 25, 1, 0.0f, 0.0f, 0.0f, 0},  // guard at (1, 2)
    {7.0f, 8.0f, {0}, ENEMY_GUARD, 25, 1, 0.0f, 0.0f, 0.0f, 0}   // guard at (7, 8)
};

int init_enemies(SDL_Renderer* renderer) {
    log_error(log_file, "[Enemies] Initializing enemy system");
    
    for (int i = 0; i < MAX_ENEMIES; i++) {
        // Keep existing position and type from predefined array
        float origX = enemies[i].x;
        float origY = enemies[i].y;
        EnemyType origType = enemies[i].type;
        int origHealth = enemies[i].health;
        
        // Generate texture
        if (create_empty_texture(renderer, &enemies[i].texture, ENEMY_WIDTH, ENEMY_HEIGHT) != 0) {
            log_error(log_file, "[Enemies] Failed to create enemy texture");
            return 1;
        }
        generate_enemy_texture(renderer, &enemies[i].texture);
        
        // Restore original values
        enemies[i].x = origX;
        enemies[i].y = origY;
        enemies[i].type = origType;
        enemies[i].health = origHealth;
        enemies[i].active = 1;
        enemies[i].angle = (float)(rand() % 360) * M_PI / 180.0f;
    }
    
    log_error(log_file, "[Enemies] Enemy system initialized successfully");
    return 0;
}

void generate_enemy_texture(SDL_Renderer* renderer __attribute__((unused)), TextureInfo* texture) {
    // Get enemy type from the offset in the enemies array
    int enemy_index = ((uintptr_t)texture - (uintptr_t)&enemies[0].texture) / sizeof(Enemy);
    EnemyType type = enemies[enemy_index].type;
    
    // Current time for animation effects
    float time = SDL_GetTicks() / 1000.0f;

    for (int y = 0; y < ENEMY_HEIGHT; y++) {
        for (int x = 0; x < ENEMY_WIDTH; x++) {
            // Calculate normalized coordinates (-1 to 1)
            float nx = (x - ENEMY_WIDTH/2.0f) / (ENEMY_WIDTH/2.0f);
            float ny = (y - ENEMY_HEIGHT/2.0f) / (ENEMY_HEIGHT/2.0f);
            float dist = sqrtf(nx*nx + ny*ny);
            float angle = atan2f(ny, nx);

            // Start with fully transparent
            Uint32 color = 0x00000000;

            switch (type) {
                case ENEMY_GUARD:
                    // Red guard with pulsing core
                    if (dist < 0.8f) {
                        float pulse = sinf(time * 3.0f) * 0.2f + 0.8f;
                        if (dist < 0.3f * pulse) {
                            // Bright core
                            color = 0xFFFF2020;
                        } else if (dist < 0.5f) {
                            // Middle ring
                            color = 0xFFCC0000;
                        } else {
                            // Outer glow
                            float alpha = (0.8f - dist) / 0.3f;
                            Uint8 a = (Uint8)(alpha * 255);
                            color = (a << 24) | 0x00800000;
                        }
                        // Add pattern
                        if (sinf(angle * 8 + time * 2) > 0.7f) {
                            color = 0xFFFF4040;
                        }
                    }
                    break;

                case ENEMY_PATROL:
                    // Yellow patrol with rotating elements
                    if (dist < 0.9f) {
                        float rotAngle = angle + time * 2;
                        float pattern = sinf(rotAngle * 6) * 0.2f;
                        if (dist + pattern < 0.7f) {
                            if (dist < 0.3f) {
                                // Core
                                color = 0xFFFFD700;
                            } else {
                                // Rotating segments
                                float segment = sinf(rotAngle * 4);
                                if (segment > 0) {
                                    color = 0xFFDAA520;
                                } else {
                                    color = 0xFFFFA500;
                                }
                            }
                            // Add pulsing effect
                            float pulse = sinf(time * 4 + dist * 5) * 0.3f + 0.7f;
                            Uint8 r = ((color >> 16) & 0xFF) * pulse;
                            Uint8 g = ((color >> 8) & 0xFF) * pulse;
                            Uint8 b = (color & 0xFF) * pulse;
                            color = (0xFF << 24) | (r << 16) | (g << 8) | b;
                        }
                    }
                    break;

                case ENEMY_BOSS:
                    // Purple boss with energy field
                    if (dist < 1.0f) {
                        float energyField = sinf(dist * 10 - time * 3) * 0.5f + 0.5f;
                        float spikes = 0.2f * sinf(angle * 8 + time);
                        
                        if (dist + spikes < 0.9f) {
                            if (dist < 0.4f) {
                                // Core
                                color = 0xFF8B008B;
                                // Add energy swirls
                                float swirl = sinf(angle * 6 + time * 4);
                                if (swirl > 0.7f) {
                                    color = 0xFFFF00FF;
                                }
                            } else if (dist < 0.7f) {
                                // Energy field
                                color = 0xFF4B0082;
                                if (energyField > 0.8f) {
                                    color = 0xFF800080;
                                }
                            } else {
                                // Outer aura
                                float alpha = (1.0f - dist) * 255;
                                Uint8 a = (Uint8)alpha;
                                color = (a << 24) | 0x00400040;
                            }
                        }
                    }
                    break;
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

// Example of spawning different enemy types
void spawn_enemy_at(float x, float y, EnemyType type) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) {
            enemies[i].x = x;
            enemies[i].y = y;
            enemies[i].type = type;
            enemies[i].active = 1;
            enemies[i].dirX = 0;
            enemies[i].dirY = 0;
            enemies[i].lastMoveTime = SDL_GetTicks();
            enemies[i].angle = (float)(rand() % 360) * M_PI / 180.0f;
            
            // Generate initial texture
            generate_enemy_texture(NULL, &enemies[i].texture);
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
    // Create z-buffer for sprite ordering
    float zBuffer[SCREEN_WIDTH];
    for (int x = 0; x < SCREEN_WIDTH; x++) {
        zBuffer[x] = player->perpWallDist[x];
    }

    // Sort enemies by distance (simple bubble sort)
    int spriteOrder[MAX_ENEMIES];
    float spriteDistance[MAX_ENEMIES];
    
    for (int i = 0; i < MAX_ENEMIES; i++) {
        spriteOrder[i] = i;
        spriteDistance[i] = ((player->x - enemies[i].x) * (player->x - enemies[i].x) +
                            (player->y - enemies[i].y) * (player->y - enemies[i].y));
    }

    // Bubble sort sprites based on distance
    for (int i = 0; i < MAX_ENEMIES - 1; i++) {
        for (int j = 0; j < MAX_ENEMIES - i - 1; j++) {
            if (spriteDistance[j] < spriteDistance[j + 1]) {
                // Swap distances
                float tempDist = spriteDistance[j];
                spriteDistance[j] = spriteDistance[j + 1];
                spriteDistance[j + 1] = tempDist;
                // Swap orders
                int tempOrder = spriteOrder[j];
                spriteOrder[j] = spriteOrder[j + 1];
                spriteOrder[j + 1] = tempOrder;
            }
        }
    }

    // Render sprites from farthest to closest
    for (int i = 0; i < MAX_ENEMIES; i++) {
        Enemy* enemy = &enemies[spriteOrder[i]];
        if (!enemy->active) continue;

        // Translate sprite position relative to camera
        float spriteX = enemy->x - player->x;
        float spriteY = enemy->y - player->y;

        // Transform sprite with the inverse camera matrix
        float invDet = 1.0f / (player->planeX * player->dirY - player->dirX * player->planeY);
        float transformX = invDet * (player->dirY * spriteX - player->dirX * spriteY);
        float transformY = invDet * (-player->planeY * spriteX + player->planeX * spriteY);

        // Don't render if behind camera
        if (transformY <= 0.1f) continue;

        // Calculate screen position and scaling
        int spriteScreenX = (int)((SCREEN_WIDTH / 2) * (1 + transformX / transformY));
        int spriteHeight = abs((int)(SCREEN_HEIGHT / transformY));
        int spriteWidth = abs((int)(SCREEN_HEIGHT / transformY));

        // Calculate drawing bounds
        int drawStartY = -spriteHeight / 2 + SCREEN_HEIGHT / 2;
        int drawEndY = spriteHeight / 2 + SCREEN_HEIGHT / 2;
        int drawStartX = -spriteWidth / 2 + spriteScreenX;
        int drawEndX = spriteWidth / 2 + spriteScreenX;

        // Clamp drawing bounds
        if (drawStartY < 0) drawStartY = 0;
        if (drawEndY >= SCREEN_HEIGHT) drawEndY = SCREEN_HEIGHT - 1;
        if (drawStartX < 0) drawStartX = 0;
        if (drawEndX >= SCREEN_WIDTH) drawEndX = SCREEN_WIDTH - 1;

        // Draw the sprite
        for (int stripe = drawStartX; stripe < drawEndX; stripe++) {
            // Check if stripe is visible and in front of walls
            if (transformY > zBuffer[stripe]) {
                int texX = (int)(256 * (stripe - (-spriteWidth / 2 + spriteScreenX)) * ENEMY_WIDTH / spriteWidth) / 256;

                // Draw the vertical stripe
                if (texX >= 0 && texX < ENEMY_WIDTH) {
                    for (int y = drawStartY; y < drawEndY; y++) {
                        int d = (y) * 256 - SCREEN_HEIGHT * 128 + spriteHeight * 128;
                        int texY = ((d * ENEMY_HEIGHT) / spriteHeight) / 256;

                        if (texY >= 0 && texY < ENEMY_HEIGHT) {
                            Uint32 color = enemy->texture.pixels[ENEMY_WIDTH * texY + texX];
                            // Only draw pixel if not transparent
                            if ((color & 0xFF000000) != 0) {
                                gfx->pixels[y * SCREEN_WIDTH + stripe] = color;
                            }
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