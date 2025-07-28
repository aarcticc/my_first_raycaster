#include "map.h"
#include "log_utils.h"
#include "enemy.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json.h>

/* Map representation:
   0 = Empty space (corridor)
   1-0 = Different wall textures
*/

int map[MAP_HEIGHT][MAP_WIDTH] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,4,0,3,0,3,3,2,2,1,1,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,4,0,1,0,1,1,4,0,4,4,3,3,0,1},
    {1,0,0,0,0,0,1,0,4,0,0,0,3,0,0,1},
    {1,0,2,2,1,1,4,4,2,0,2,2,4,0,0,1},
    {1,0,0,0,1,0,0,0,2,0,2,0,0,0,0,1},
    {1,0,1,1,1,1,1,0,2,0,3,3,4,4,0,1},
    {1,0,0,0,0,0,0,0,2,0,3,0,4,0,0,1},
    {1,0,4,4,2,0,4,4,4,4,1,1,3,0,0,1},
    {1,0,4,0,0,0,4,0,0,0,1,0,3,0,0,1},
    {1,0,1,1,3,0,2,2,1,0,2,0,3,0,0,1},
    {1,0,1,0,0,0,2,0,1,0,2,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

int is_valid_position(float x, float y) {
    int mapX = (int)x;
    int mapY = (int)y;
    
    if (mapX < 0 || mapX >= MAP_WIDTH || mapY < 0 || mapY >= MAP_HEIGHT) {
        return 0;
    }
    
    return map[mapY][mapX] == 0;
}

void find_nearest_empty_space(float* x, float* y) {
    int currentX = (int)*x;
    int currentY = (int)*y;
    float minDistance = INFINITY;
    float bestX = *x;
    float bestY = *y;

    // Search in increasing spiral pattern
    for(int radius = 1; radius < MAP_WIDTH && radius < MAP_HEIGHT; radius++) {
        for(int dx = -radius; dx <= radius; dx++) {
            for(int dy = -radius; dy <= radius; dy++) {
                int testX = currentX + dx;
                int testY = currentY + dy;
                
                if (testX >= 0 && testX < MAP_WIDTH && 
                    testY >= 0 && testY < MAP_HEIGHT && 
                    map[testY][testX] == 0) {
                    
                    float distance = sqrtf((float)(dx * dx + dy * dy));
                    if (distance < minDistance) {
                        minDistance = distance;
                        bestX = testX + 0.5f; // Center of the cell
                        bestY = testY + 0.5f;
                    }
                }
            }
        }
        // If we found a valid position, stop searching
        if (minDistance != INFINITY) break;
    }

    *x = bestX;
    *y = bestY;
}

int load_custom_map(const char* filename) {
    json_object *root;
    json_object *grid, *enemies_array;
    
    root = json_object_from_file(filename);
    if (!root) {
        log_error(log_file, "[Map] Failed to parse JSON file");
        return 1;
    }

    // Get and validate dimensions
    json_object *width_obj, *height_obj;
    if (json_object_object_get_ex(root, "width", &width_obj) &&
        json_object_object_get_ex(root, "height", &height_obj)) {
        int width = json_object_get_int(width_obj);
        int height = json_object_get_int(height_obj);
        if (width != MAP_WIDTH || height != MAP_HEIGHT) {
            log_error(log_file, "[Map] Map dimensions mismatch");
            json_object_put(root);
            return 1;
        }
    }

    // Get the grid array
    if (!json_object_object_get_ex(root, "grid", &grid)) {
        log_error(log_file, "[Map] No grid found in JSON");
        json_object_put(root);
        return 1;
    }

    // Load the map grid
    for (int y = 0; y < MAP_HEIGHT; y++) {
        json_object *row = json_object_array_get_idx(grid, y);
        if (!row) continue;
        
        for (int x = 0; x < MAP_WIDTH; x++) {
            json_object *cell = json_object_array_get_idx(row, x);
            if (!cell) continue;
            map[y][x] = json_object_get_int(cell);
        }
    }

    // Reset existing enemies
    for (int i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].active = 0;
    }

    // Load enemies if present
    if (json_object_object_get_ex(root, "enemies", &enemies_array)) {
        int enemy_count = json_object_array_length(enemies_array);
        enemy_count = enemy_count < MAX_ENEMIES ? enemy_count : MAX_ENEMIES;

        for (int i = 0; i < enemy_count; i++) {
            json_object *enemy = json_object_array_get_idx(enemies_array, i);
            json_object *x, *y, *type, *health;

            if (json_object_object_get_ex(enemy, "x", &x) &&
                json_object_object_get_ex(enemy, "y", &y) &&
                json_object_object_get_ex(enemy, "type", &type) &&
                json_object_object_get_ex(enemy, "health", &health)) {

                // Get enemy type string and convert to enum
                const char* type_str = json_object_get_string(type);
                EnemyType enemy_type = ENEMY_GUARD; // default
                if (strcmp(type_str, "patrol") == 0) enemy_type = ENEMY_PATROL;
                if (strcmp(type_str, "boss") == 0) enemy_type = ENEMY_BOSS;

                // Set enemy properties
                enemies[i].x = json_object_get_double(x);
                enemies[i].y = json_object_get_double(y);
                enemies[i].type = enemy_type;
                enemies[i].health = json_object_get_int(health);
                enemies[i].active = 1;
                enemies[i].angle = (float)(rand() % 360) * M_PI / 180.0f;
            }
        }
        log_error(log_file, "[Map] Enemies loaded from JSON");
    }

    json_object_put(root);
    return 0;
}