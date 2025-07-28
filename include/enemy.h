#ifndef ENEMY_H
#define ENEMY_H

#include <SDL2/SDL.h>
#include "texture.h"
#include "raycaster.h"  // Added for Player and Graphics types
#include "map.h"        // Added for MAP_WIDTH and MAP_HEIGHT

#define MAX_ENEMIES 3  // Changed from 10 to match map.txt
#define ENEMY_WIDTH 32
#define ENEMY_HEIGHT 32
#define ENEMY_SPEED 0.03f
#define ENEMY_CATCH_DISTANCE 1.0f
#define ENEMY_MOVE_DELAY 16  // milliseconds between moves

typedef enum {
    ENEMY_GUARD,
    ENEMY_PATROL,
    ENEMY_BOSS
} EnemyType;

typedef struct {
    float x, y;           // Position
    TextureInfo texture;  // Enemy texture
    EnemyType type;      // Enemy type
    int health;          // Health points
    int active;          // Whether enemy is alive
    float angle;         // View direction
    float dirX, dirY;    // Direction vector
    Uint32 lastMoveTime; // Time of last movement
} Enemy;

extern Enemy enemies[MAX_ENEMIES];

// Initialize enemy system and generate textures
int init_enemies(SDL_Renderer* renderer);

// Generate a procedural texture for an enemy
void generate_enemy_texture(SDL_Renderer* renderer, TextureInfo* texture);

// Clean up enemy resources
void destroy_enemies(void);

// Add enemy spawn position to map
void spawn_enemy_at(float x, float y);

// Update enemy positions (called each frame)
void update_enemies(const Player* player, int map[MAP_HEIGHT][MAP_WIDTH]);

// Check if player is caught by any enemy
int is_player_caught(const Player* player);

// Render enemies in the world
void render_enemies(Graphics* gfx, const Player* player);

// Render caught indicator (red dot)
void render_caught_indicator(Graphics* gfx);

#endif
