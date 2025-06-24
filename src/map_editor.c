#include "map.h"
#include "texture.h"
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>

#define CELL_SIZE 24
#define SIDEBAR_WIDTH 160
#define WIN_WIDTH (MAP_WIDTH * CELL_SIZE + SIDEBAR_WIDTH)
#define WIN_HEIGHT (MAP_HEIGHT * CELL_SIZE)
#define WALL_BTN_SIZE 48
#define WALL_BTN_MARGIN 16
#define WALL_BTN_X (MAP_WIDTH * CELL_SIZE + 32)
#define WALL_BTN_Y_START 32
#define SAVE_BTN_X (MAP_WIDTH * CELL_SIZE + 20)
#define SAVE_BTN_Y (WIN_HEIGHT - 80)
#define SAVE_BTN_W 120
#define SAVE_BTN_H 40

static int current_tile = 1; // Wall type to place

// Helper: draw grid as before
static void draw_grid(SDL_Renderer* renderer) {
    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            SDL_Rect cell = { x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE };
            if (map[y][x] > 0) {
                // Preview with wall texture if available, else color
                int idx = map[y][x] - 1;
                if (idx >= 0 && idx < NUM_WALL_TEXTURES && validate_texture(&wall_textures[idx])) {
                    SDL_RenderCopy(renderer, wall_textures[idx].texture, NULL, &cell);
                } else {
                    SDL_SetRenderDrawColor(renderer, 100 + 30 * idx, 100, 100, 255);
                    SDL_RenderFillRect(renderer, &cell);
                }
            } else {
                SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
                SDL_RenderFillRect(renderer, &cell);
            }
            SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
            SDL_RenderDrawRect(renderer, &cell);
        }
    }
}

static void draw_save_button(SDL_Renderer* renderer) {
    SDL_Rect btn = { SAVE_BTN_X, SAVE_BTN_Y, SAVE_BTN_W, SAVE_BTN_H };
    
    // Button background with gradient effect
    SDL_SetRenderDrawColor(renderer, 40, 180, 40, 255);  // Main green color
    SDL_RenderFillRect(renderer, &btn);
    
    // Top highlight
    SDL_SetRenderDrawColor(renderer, 60, 200, 60, 255);
    SDL_Rect highlight = { btn.x + 1, btn.y + 1, btn.w - 2, 2 };
    SDL_RenderFillRect(renderer, &highlight);
    
    // Bottom shadow
    SDL_SetRenderDrawColor(renderer, 20, 160, 20, 255);
    SDL_Rect shadow = { btn.x + 1, btn.y + btn.h - 3, btn.w - 2, 2 };
    SDL_RenderFillRect(renderer, &shadow);
    
    // Border
    SDL_SetRenderDrawColor(renderer, 0, 100, 0, 255);
    SDL_RenderDrawRect(renderer, &btn);
    
    // Draw "SAVE" text using multiple rectangles
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    
    // S
    SDL_Rect s1 = { btn.x + 20, btn.y + 12, 20, 3 };  // Top horizontal
    SDL_Rect s2 = { btn.x + 20, btn.y + 12, 3, 8 };   // Top vertical
    SDL_Rect s3 = { btn.x + 20, btn.y + 19, 20, 3 };  // Middle horizontal
    SDL_Rect s4 = { btn.x + 37, btn.y + 21, 3, 8 };   // Bottom vertical
    SDL_Rect s5 = { btn.x + 20, btn.y + 28, 20, 3 };  // Bottom horizontal
    
    // A
    SDL_Rect a1 = { btn.x + 45, btn.y + 12, 3, 19 };  // Left vertical
    SDL_Rect a2 = { btn.x + 45, btn.y + 12, 15, 3 };  // Top horizontal
    SDL_Rect a3 = { btn.x + 57, btn.y + 12, 3, 19 };  // Right vertical
    SDL_Rect a4 = { btn.x + 45, btn.y + 19, 15, 3 };  // Middle horizontal
    
    // V
    SDL_Rect v1 = { btn.x + 65, btn.y + 12, 3, 13 };  // Left vertical
    SDL_Rect v2 = { btn.x + 65, btn.y + 24, 8, 3 };   // Bottom diagonal
    SDL_Rect v3 = { btn.x + 77, btn.y + 12, 3, 13 };  // Right vertical
    
    // E
    SDL_Rect e1 = { btn.x + 85, btn.y + 12, 15, 3 };  // Top horizontal
    SDL_Rect e2 = { btn.x + 85, btn.y + 12, 3, 19 };  // Vertical
    SDL_Rect e3 = { btn.x + 85, btn.y + 19, 15, 3 };  // Middle horizontal
    SDL_Rect e4 = { btn.x + 85, btn.y + 28, 15, 3 };  // Bottom horizontal
    
    // Render all parts of the letters
    SDL_RenderFillRect(renderer, &s1);
    SDL_RenderFillRect(renderer, &s2);
    SDL_RenderFillRect(renderer, &s3);
    SDL_RenderFillRect(renderer, &s4);
    SDL_RenderFillRect(renderer, &s5);
    
    SDL_RenderFillRect(renderer, &a1);
    SDL_RenderFillRect(renderer, &a2);
    SDL_RenderFillRect(renderer, &a3);
    SDL_RenderFillRect(renderer, &a4);
    
    SDL_RenderFillRect(renderer, &v1);
    SDL_RenderFillRect(renderer, &v2);
    SDL_RenderFillRect(renderer, &v3);
    
    SDL_RenderFillRect(renderer, &e1);
    SDL_RenderFillRect(renderer, &e2);
    SDL_RenderFillRect(renderer, &e3);
    SDL_RenderFillRect(renderer, &e4);
}

// Checks if mouse is over the save button
static bool mouse_in_save_btn(int mx, int my) {
    return mx >= SAVE_BTN_X && mx < SAVE_BTN_X + SAVE_BTN_W &&
           my >= SAVE_BTN_Y && my < SAVE_BTN_Y + SAVE_BTN_H;
}

// Draw wall selection buttons with texture previews
static void draw_wall_buttons(SDL_Renderer* renderer) {
    for (int i = 0; i < NUM_WALL_TEXTURES; ++i) {
        int btn_y = WALL_BTN_Y_START + i * (WALL_BTN_SIZE + WALL_BTN_MARGIN);
        SDL_Rect btn = { WALL_BTN_X, btn_y, WALL_BTN_SIZE, WALL_BTN_SIZE };
        // Highlight if selected
        if (current_tile == i + 1) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
            SDL_RenderDrawRect(renderer, &btn);
        } else {
            SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255);
            SDL_RenderDrawRect(renderer, &btn);
        }
        // Draw texture preview if loaded
        if (validate_texture(&wall_textures[i])) {
            SDL_RenderCopy(renderer, wall_textures[i].texture, NULL, &btn);
        } else {
            SDL_SetRenderDrawColor(renderer, 100 + 30 * i, 100, 100, 255);
            SDL_RenderFillRect(renderer, &btn);
        }
    }
    // Draw "empty" button (eraser)
    int btn_y = WALL_BTN_Y_START + NUM_WALL_TEXTURES * (WALL_BTN_SIZE + WALL_BTN_MARGIN);
    SDL_Rect btn = { WALL_BTN_X, btn_y, WALL_BTN_SIZE, WALL_BTN_SIZE };
    if (current_tile == 0) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        SDL_RenderDrawRect(renderer, &btn);
    } else {
        SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255);
        SDL_RenderDrawRect(renderer, &btn);
    }
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
    SDL_RenderFillRect(renderer, &btn);
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderDrawLine(renderer, btn.x + 8, btn.y + 8, btn.x + WALL_BTN_SIZE - 8, btn.y + WALL_BTN_SIZE - 8);
    SDL_RenderDrawLine(renderer, btn.x + 8, btn.y + WALL_BTN_SIZE - 8, btn.x + WALL_BTN_SIZE - 8, btn.y + 8);
}

// Checks if mouse is over a wall button, returns index or -1
static int wall_btn_at(int mx, int my) {
    for (int i = 0; i < NUM_WALL_TEXTURES; ++i) {
        int btn_y = WALL_BTN_Y_START + i * (WALL_BTN_SIZE + WALL_BTN_MARGIN);
        if (mx >= WALL_BTN_X && mx < WALL_BTN_X + WALL_BTN_SIZE &&
            my >= btn_y && my < btn_y + WALL_BTN_SIZE) {
            return i + 1;
        }
    }
    // Eraser button
    int btn_y = WALL_BTN_Y_START + NUM_WALL_TEXTURES * (WALL_BTN_SIZE + WALL_BTN_MARGIN);
    if (mx >= WALL_BTN_X && mx < WALL_BTN_X + WALL_BTN_SIZE &&
        my >= btn_y && my < btn_y + WALL_BTN_SIZE) {
        return 0;
    }
    return -1;
}

int main(int argc, char* argv[]) {
    // Always load wall textures for preview
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Map Editor", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIN_WIDTH, WIN_HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    init_textures(renderer);

    // Load map from argument or zero
    const char* mapfile = argc > 1 ? argv[1] : "maps/default.map";
    if (!load_map(mapfile, map)) {
        for (int y = 0; y < MAP_HEIGHT; ++y)
            for (int x = 0; x < MAP_WIDTH; ++x)
                map[y][x] = 0;
    }

    bool running = true;
    bool mouse_down = false;
    int last_cell_x = -1, last_cell_y = -1;

    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE) running = false;
                if (e.key.keysym.sym == SDLK_s) save_map(mapfile, map);
                if (e.key.keysym.sym >= SDLK_1 && e.key.keysym.sym <= SDLK_0 + NUM_WALL_TEXTURES)
                    current_tile = e.key.keysym.sym - SDLK_0;
                if (e.key.keysym.sym == SDLK_0) current_tile = 0;
            }
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                int mx = e.button.x, my = e.button.y;
                mouse_down = true;
                last_cell_x = last_cell_y = -1;
                if (mouse_in_save_btn(mx, my)) {
                    save_map(mapfile, map);
                } else {
                    int btn = wall_btn_at(mx, my);
                    if (btn != -1) {
                        current_tile = btn;
                    } else if (mx < MAP_WIDTH * CELL_SIZE && my < MAP_HEIGHT * CELL_SIZE) {
                        int cell_x = mx / CELL_SIZE, cell_y = my / CELL_SIZE;
                        if (e.button.button == SDL_BUTTON_LEFT) {
                            map[cell_y][cell_x] = current_tile;
                        } else if (e.button.button == SDL_BUTTON_RIGHT) {
                            map[cell_y][cell_x] = 0;
                        }
                        last_cell_x = cell_x;
                        last_cell_y = cell_y;
                    }
                }
            }
            if (e.type == SDL_MOUSEBUTTONUP) {
                mouse_down = false;
                last_cell_x = last_cell_y = -1;
            }
            if (e.type == SDL_MOUSEMOTION && mouse_down) {
                int mx = e.motion.x, my = e.motion.y;
                if (mx < MAP_WIDTH * CELL_SIZE && my < MAP_HEIGHT * CELL_SIZE) {
                    int cell_x = mx / CELL_SIZE, cell_y = my / CELL_SIZE;
                    if (cell_x != last_cell_x || cell_y != last_cell_y) {
                        if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
                            map[cell_y][cell_x] = current_tile;
                        } else if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT)) {
                            map[cell_y][cell_x] = 0;
                        }
                        last_cell_x = cell_x;
                        last_cell_y = cell_y;
                    }
                }
            }
        }
        SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
        SDL_RenderClear(renderer);
        draw_grid(renderer);
        // Sidebar background
        SDL_Rect sidebar = { MAP_WIDTH * CELL_SIZE, 0, SIDEBAR_WIDTH, WIN_HEIGHT };
        SDL_SetRenderDrawColor(renderer, 32, 32, 32, 255);
        SDL_RenderFillRect(renderer, &sidebar);
        draw_wall_buttons(renderer);
        draw_save_button(renderer);
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    destroy_textures();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}