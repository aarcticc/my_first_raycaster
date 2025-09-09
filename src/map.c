#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
//! Standart C Library
#include "log_utils.h"
#include "map.h"
#include "json.h"
//! Project Headers

static struct json_value_s *find_in_object(struct json_object_s *obj, const char *key) {
    for (struct json_object_element_s *el = obj->start; el; el = el->next) {
        if (el->name && el->name->string && strcmp(el->name->string, key) == 0) {
            return el->value;
        }
    }
    return NULL;
}

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
    FILE *f = fopen(filename, "rb");
    if (!f) {
        log_error(log_file, "[Map] Failed to open JSON file");
        return 1;
    }

    if (fseek(f, 0, SEEK_END) != 0) {
        fclose(f);
        log_error(log_file, "[Map] Failed to read JSON file");
        return 1;
    }

    long fsize = ftell(f);
    if (fsize < 0) {
        fclose(f);
        log_error(log_file, "[Map] Failed to read JSON file size");
        return 1;
    }
    rewind(f);

    char *buffer = (char*)malloc((size_t)fsize);
    if (!buffer) {
        fclose(f);
        log_error(log_file, "[Map] Out of memory reading JSON");
        return 1;
    }

    if (fread(buffer, 1, (size_t)fsize, f) != (size_t)fsize) {
        free(buffer);
        fclose(f);
        log_error(log_file, "[Map] Failed to read JSON file contents");
        return 1;
    }

    fclose(f);

    struct json_parse_result_s res;
    struct json_value_s *root = json_parse_ex(buffer, (size_t)fsize, json_parse_flags_default, NULL, NULL, &res);
    if (!root) {
        free(buffer);
        log_error(log_file, "[Map] Failed to parse JSON file");
        return 1;
    }

    /* root must be an object */
    struct json_object_s *root_obj = json_value_as_object(root);
    if (!root_obj) {
        free(root);
        free(buffer);
        log_error(log_file, "[Map] JSON root is not an object");
        return 1;
    }

    /* Validate dimensions if present */
    struct json_value_s *width_val = find_in_object(root_obj, "width");
    struct json_value_s *height_val = find_in_object(root_obj, "height");
    if (width_val && height_val) {
        struct json_number_s *wnum = json_value_as_number(width_val);
        struct json_number_s *hnum = json_value_as_number(height_val);
        if (wnum && hnum) {
            int width = atoi(wnum->number);
            int height = atoi(hnum->number);
            if (width != MAP_WIDTH || height != MAP_HEIGHT) {
                free(root);
                free(buffer);
                log_error(log_file, "[Map] Map dimensions mismatch");
                return 1;
            }
        }
    }

    /* Load the grid */
    struct json_value_s *grid_val = find_in_object(root_obj, "grid");
    if (!grid_val) {
        free(root);
        free(buffer);
        log_error(log_file, "[Map] No grid found in JSON");
        return 1;
    }

    struct json_array_s *grid_arr = json_value_as_array(grid_val);
    if (!grid_arr) {
        free(root);
        free(buffer);
        log_error(log_file, "[Map] grid is not an array");
        return 1;
    }

    struct json_array_element_s *row_el = grid_arr->start;
    for (int y = 0; y < MAP_HEIGHT; y++) {
        if (!row_el) break;
        struct json_value_s *row_val = row_el->value;
        struct json_array_s *row_arr = json_value_as_array(row_val);
        if (row_arr) {
            struct json_array_element_s *cell_el = row_arr->start;
            for (int x = 0; x < MAP_WIDTH; x++) {
                if (!cell_el) break;
                struct json_value_s *cell_val = cell_el->value;
                struct json_number_s *num = json_value_as_number(cell_val);
                if (num && num->number) {
                    map[y][x] = atoi(num->number);
                }
                cell_el = cell_el->next;
            }
        }
        row_el = row_el->next;
    }

    free(root);
    free(buffer);
    return 0;
}