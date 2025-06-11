#ifndef MAP_H  // Header guard to prevent multiple inclusion
#define MAP_H

// Define the dimensions of the game map
// These are preprocessor constants that will be replaced at compile time
// MAP_WIDTH: Number of cells across (X direction)
// MAP_HEIGHT: Number of cells down (Y direction)
// Values must be integers as they're used for array dimensions
#define MAP_WIDTH 20
#define MAP_HEIGHT 20

// Declare an external 2D array that will hold the map data
// 'extern' keyword indicates this array is defined in another source file
// Each cell in the array represents a wall (1) or empty space (0)
// The array is fixed size: MAP_HEIGHT rows by MAP_WIDTH columns
extern int map[MAP_HEIGHT][MAP_WIDTH];

// Function to find nearest empty space to a given position
void find_nearest_empty_space(float* x, float* y);

// Function to validate spawn position
int is_valid_position(float x, float y);

#endif  // End of header guard