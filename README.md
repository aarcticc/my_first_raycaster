# Raycaster (C / SDL2)

A lightweight 2D raycasting engine written in pure C using SDL2. Designed for educational use and prototyping, this project demonstrates core techniques used in early 3D games like *Wolfenstein 3D*.

---

## 🚀 Features

- Real-time raycasting engine in C
- Configurable 2D map (e.g., 16×16 grid)
- Player movement with arrow keys
- Floor and ceiling rendering
- Wall rendering with depth shading
- Modular source structure and Makefile
- Cross-platform build (Linux/macOS/Windows via MinGW)

---

## 📷 Screenshots

> soon

---

## 🧩 Requirements

- GCC or Clang
- [SDL2](https://www.libsdl.org/)
- *(Optional)* SDL2_image for future texture support

---

## 🔧 Build Instructions (Linux)

```bash
# Install dependencies
sudo apt update
sudo apt install build-essential libsdl2-dev

# Clone repository
git clone https://github.com/yourusername/raycaster.git
cd raycaster

# Build and run
make
make run

---

## 🪟 Build Instructions (Windows 10/11 with MinGW)

1. Install MinGW-w64


2. Install SDL2 development libraries for MinGW


3. Add SDL2 include/ and lib/ paths to your Makefile


4. Cross-compile using:

̀̀̀``powershell
x86_64-w64-mingw32-gcc -o raycaster.exe src/*.c -Iinclude -Llib -lmingw32 -lSDL2main -lSDL2 -mwindows


---

## 🔄 Roadmap

[x] Floor and ceiling rendering

[x] Basic wall shading

[ ] Texture mapping (128x128 PNG or BMP)

[ ] Map editor

[ ] Portals and triggers

[ ] Sprite support (e.g., objects/enemies)



---

## 🔢 Versioning

Current: v0.1.0-alpha
Semantic versioning with stage tags: alpha, beta, release.


---

📁 Project Structure

raycaster/
├── src/           # C source files
├── include/       # Header files
├── bin/           # Compiled object files
├── textures/      # Wall/floor textures (128x128)
├── Makefile       # Build script
└── README.md


---

🧑‍💻 Author

¡arctic¿
github.com/aarcticc


---

📄 License

This project is licensed under the MIT License — see LICENSE for details.