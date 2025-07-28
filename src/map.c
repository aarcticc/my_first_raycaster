#include "map.h"
#include "log_utils.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    FILE* file = fopen(filename, "r");
    if (!file) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), 
                "[Map] Failed to open map file: %s", filename);
        log_error(log_file, error_msg);
        return 1;
    }

    log_error(log_file, "[Map] Starting to parse map file");
    
    char buffer[1024];
    int in_grid = 0;
    int row = 0;

    while (fgets(buffer, sizeof(buffer), file)) {
        if (strstr(buffer, "\"grid\":")) {
            in_grid = 1;
            continue;
        }

        if (in_grid) {
            if (strstr(buffer, "]")) {
                break;
            }

            // Parse grid row
            char* token = strtok(buffer, "[ ,]\n");
            int col = 0;
            
            while (token && col < MAP_WIDTH) {
                if (*token >= '0' && *token <= '9') {  // Valid number
                    map[row][col] = atoi(token);
                    col++;
                }
                token = strtok(NULL, "[ ,]\n");
            }
            
            row++;
            if (row >= MAP_HEIGHT) break;
        }
    }

    fclose(file);
    
    // Add logging for map validation
    log_error(log_file, "[Map] Validating map borders");
    
    // Validate map borders
    for (int x = 0; x < MAP_WIDTH; x++) {
        if (map[0][x] == 0 || map[MAP_HEIGHT-1][x] == 0) {
            log_error(log_file, "[Map] Invalid map: horizontal borders must be solid walls");
            return 1;
        }
    }
    for (int y = 0; y < MAP_HEIGHT; y++) {
        if (map[y][0] == 0 || map[y][MAP_WIDTH-1] == 0) {
            log_error(log_file, "[Map] Invalid map: vertical borders must be solid walls");
            return 1;
        }
    }

    log_error(log_file, "[Map] Map validation successful");
    return 0;
}