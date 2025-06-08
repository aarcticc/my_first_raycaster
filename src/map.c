#include "map.h"

/* Map representation:
   - The map is a 2D array where:
     0 = Empty space/corridor that the player can move through
     1 = Wall that blocks player movement
   - The map is surrounded by walls (1's) to prevent the player from going out of bounds
   - The center has a 2x2 pillar (positions 4,4 to 5,5)
   - The outer border is solid walls for containment
*/

int map[MAP_HEIGHT][MAP_WIDTH] = {
    {2,2,2,2,2,2,2,2,2,2},
    {1,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,3,2,0,0,0,1},
    {1,0,0,0,1,4,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1}
};
