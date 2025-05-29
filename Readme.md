# Raycaster

A simple raycasting engine implemented in C. This project demonstrates the fundamentals of 2D raycasting, similar to early 3D games like Wolfenstein 3D.

## Features

- 2D grid-based map rendering
- Real-time player movement and rotation
- Wall collision detection
- Basic texture support (optional)
- Configurable map layouts

## Getting Started

### Prerequisites

- C compiler (e.g., gcc or clang)
- `SDL2` library

Install dependencies (on Ubuntu/Debian):

```bash
sudo apt-get install libsdl2-dev
```

### Building and Running the Project

```bash
gcc -o raycaster main.c raycaster.c map.c -lSDL2
./raycaster
```

- **main.c**: Entry point, handles game loop and input.
- **raycaster.c**: Core raycasting logic.
- **map.c**: Map data and rendering.
- **textures/**: (Optional) Wall textures.

## Usage

- **W/A/S/D**: Move forward/left/back/right
- **Arrow keys**: Rotate view

## Documentation

### Main Functions

#### `void cast_rays(Player *player, int **map_data, Ray *rays)`

Casts rays from the player's position and fills an array of wall hit distances.

#### `void move_player(Player *player, int direction, int **map_data)`

Moves the player in the specified direction, handling collision.

#### `void draw_scene(SDL_Renderer *renderer, Ray *rays, Texture *textures)`

Renders the 3D scene based on raycasting results.

### Map Format

Maps are defined as 2D arrays, where `1` represents a wall and `0` is empty space.

```c
int map_data[5][5] = {
    {1,1,1,1,1},
    {1,0,0,0,1},
    {1,0,1,0,1},
    {1,0,0,0,1},
    {1,1,1,1,1}
};
```

## License

MIT License

## Credits

Inspired by classic raycasting tutorials and open-source projects.