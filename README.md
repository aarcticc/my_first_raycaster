# my_first_raycaster

A small and lightweight raycaster made in C. This project implements a classic Wolfenstein 3D-style raycasting engine, providing a pseudo-3D first-person perspective from a 2D map.

## Table of Contents

- [About](#about)
- [Features](#features)
- [Dependencies](#dependencies)
- [Installation](#installation)
- [Usage](#usage)
- [Controls](#controls)
- [How It Works](#how-it-works)
- [Project Structure](#project-structure)
- [Learning Resources](#learning-resources)
- [License](#license)
- [Note](#note)

## Disclaimer

For now, I excuse myself, that 90% of the Code is either written by AI tools or lend from other sources. Aswell in the future all C++ in this project will cede to exist, when i find a way, around that giant json.h file. Trust me it will be replaced, if I don't abandon the project.

## About

This raycaster is an educational project that demonstrates the fundamentals of 3D graphics rendering using the raycasting technique popularized by classic games like Wolfenstein 3D. The project is designed to be simple, lightweight, and easy to understand, making it perfect for learning both C programming and graphics rendering concepts.

Raycasting is a rendering technique that creates a 3D perspective in a 2D environment by casting rays from the player's viewpoint into the game world and calculating wall distances to determine rendering height.

## Features

- **Pseudo-3D rendering** using raycasting algorithm
- **First-person perspective** navigation
- **Real-time rendering** with smooth movement
- **Lightweight implementation** in pure C
- **Educational codebase** - well-structured for learning
- **2D map-based world** representation
- **Wall collision detection**
- **Texture mapping** (if applicable)

## Dependencies

To build and run this raycaster, you'll need the following dependencies:

### Required

- **C Compiler** (GCC recommended or Clang)
  - GCC 4.8 or later
  - Clang 3.4 or later
  
- **Make** - Build automation tool
  - GNU Make 3.81 or later

- **Graphics Library** (one of the following):
  - **SDL2** (Simple DirectMedia Layer 2)
    - Version 2.0.0 or later
    - Used for window creation, rendering, and input handling

### Optional

- **Math Library** (`libm`)
  - Usually included with standard C library
  - Required for trigonometric functions

## Installation

### Linux (Debian and Debian based)

```bash
# Install dependencies
sudo apt-get update
sudo apt-get install gcc make libsdl2-dev

# Clone the repository
git clone https://github.com/aarcticc/my_first_raycaster.git
cd my_first_raycaster

# Build the project
make

# Run the raycaster
./raycaster
```

### Linux (Fedora/RHEL)

```bash
# Install dependencies
sudo dnf install gcc make SDL2-devel

# Clone the repository
git clone https://github.com/aarcticc/my_first_raycaster.git
cd my_first_raycaster

# Build the project (see .txt)
make

# Run the raycaster
./raycaster
```

### macOS

```bash
# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install gcc make sdl2

# Clone the repository
git clone https://github.com/aarcticc/my_first_raycaster.git
cd my_first_raycaster

# Build the project
make

# Run the raycaster
./raycaster
```

### Windows (MinGW)

```bash
# Install MinGW and SDL2 (or use MSYS2)
# Download SDL2 development libraries from https://www.libsdl.org/download-2.0.php

# Clone the repository
git clone https://github.com/aarcticc/my_first_raycaster.git
cd my_first_raycaster

# Build the project
make

# Run the raycaster
raycaster.exe
```

## Usage

### Basic Execution

```bash
# Run with default map
./raycaster

# Run with custom map file (if supported)
./raycaster maps/level1.map
```

### Building from Source

```bash
# Clean previous builds
make clean
```

## Controls

| Key | Action |
|-----|--------|
| `↑` | Move forward |
| `↓` | Move backward |
| `←` | Rotate camera left |
| `→` | Rotate camera right |

## How It Works

### Raycasting Fundamentals

1. **Map Representation**: The world is represented as a 2D grid where each cell is either empty (0) or a wall (1)

2. **Ray Casting**: For each vertical strip of the screen:
   - Cast a ray from the player's position
   - Calculate the angle based on the player's viewing direction and field of view
   - Step through the map until a wall is hit
   - Calculate the distance to the wall

3. **Wall Rendering**:
   - Convert distance to wall height on screen
   - Apply perspective correction (fisheye correction)
   - Draw vertical strips to create the 3D illusion

4. **Performance Optimization**:
   - DDA (Digital Differential Analysis) algorithm for ray-wall intersection
   - Fixed-point arithmetic where applicable
   - Efficient distance calculations

### Mathematical Foundation

The raycaster uses basic trigonometry:
- **Ray angle**: `player_angle + (column / screen_width) * FOV`
- **Distance calculation**: Euclidean distance with fisheye correction
- **Wall height**: `(WALL_HEIGHT / distance) * distance_to_projection_plane`

## Project Structure

```
my_first_raycaster/
├── src/
│   ├── main.c              # Entry point and main game loop
│   ├── raycaster.c         # Core raycasting algorithm
│   ├── render.c            # Rendering functions
│   ├── player.c            # Player movement and rotation
│   ├── map.c               # Map loading and collision detection
│   └── utils.c             # Utility functions
├── include/
│   ├── raycaster.h         # Main header file
│   ├── player.h            # Player structure and functions
│   ├── map.h               # Map structure and functions
│   └── config.h            # Configuration constants
├── maps/
│   ├── level1.map          # Sample map files
│   └── level2.map
├── Makefile                # Build configuration
├── README.md               # This file
└── LICENSE                 # License information
```

## Learning Resources

If you want to learn more about raycasting and use this code for educational purposes:

### Recommended Tutorials

- **Lode's Computer Graphics Tutorial**: [https://lodev.org/cgtutor/raycasting.html](https://lodev.org/cgtutor/raycasting.html)
  - Comprehensive raycasting tutorial with detailed explanations
  - Covers basic to advanced techniques

- **Permadi's Ray-Casting Tutorial**: Classic resource explaining the mathematics

- **Fabien Sanglard's Game Engine Black Books**: Deep dive into Wolfenstein 3D and DOOM

### Key Concepts to Understand

- **Trigonometry**: sine, cosine, tangent functions
- **Vector mathematics**: dot products, vector lengths
- **Grid-based collision detection**
- **Perspective projection**
- **DDA algorithm** for line drawing
- **Bresenham's line algorithm** (alternative approach)

### Extending This Project

Ideas for learning and expansion:

1. **Add textured walls**: Load and map textures to walls
2. **Floor and ceiling rendering**: Implement floor-casting
3. **Sprites**: Add 2D sprites in 3D space (enemies, items)
4. **Lighting effects**: Distance-based shading
5. **Multiple wall heights**: Variable height walls
6. **Doors**: Animated door objects
7. **Minimap**: Top-down 2D view of the map
8. **Sound effects**: Integrate audio library

## License

This project is open source and available for educational purposes. Feel free to copy, modify, and use the code to learn C programming and graphics rendering.

## Note

> **Educational Purpose**: This raycaster is designed as a learning tool. The code prioritizes clarity and understanding over optimization. It's perfect for:
> - Learning C programming language
> - Understanding graphics rendering fundamentals
> - Exploring game development basics
> - Studying classic game engine techniques

> **Code Quality**: While functional, this is a learning project. Consider it a starting point for understanding raycasting rather than production-ready code.

> **Performance**: This implementation focuses on demonstrating the algorithm clearly. For production use, consider more optimized approaches or modern graphics APIs (OpenGL, Vulkan, DirectX).

## Contributing

Feel free to fork this project and experiment with it! Some ideas:

- Add new features
- Optimize the rendering pipeline
- Improve code documentation
- Create new maps
- Add texture support
- Implement advanced features

## Acknowledgments

- Inspired by Wolfenstein 3D and the classic raycasting technique
- Built for educational purposes to help others learn
- Thanks to the game development community for excellent tutorials

## Troubleshooting

### Common Issues

**Black screen on launch:**
- Check if SDL2 is properly installed
- Verify the map file is valid
- Ensure graphics drivers are up to date

**Compilation errors:**
- Verify all dependencies are installed
- Check GCC/Clang version compatibility
- Review Makefile paths for libraries

**Performance issues:**
- Lower the screen resolution
- Reduce the number of rays cast
- Check for infinite loops in ray casting code

**Controls not responding:**
- Verify SDL2 event handling is working
- Check keyboard input polling

---

For questions or issues, please open an issue on the GitHub repository.