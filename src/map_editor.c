#include "map.h"
#include "log.h"
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>

#define CELL_SIZE 24
#define WIN_WIDTH (MAP_WIDTH * CELL_SIZE)
#define WIN_HEIGHT (MAP_HEIGHT * CELL_SIZE)

static Map map;
static int current_tile = 1; // Wall type to place

static void draw_grid(SDL_Renderer* renderer) {
    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            SDL_Rect cell = { x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE };
            if (map.data[x][y] > 0) {
                SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
            } else {
                SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
            }
            SDL_RenderFillRect(renderer, &cell);
            SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
            SDL_RenderDrawRect(renderer, &cell);
        }
    }
}

int main(int argc, char* argv[]) {
    const char* mapfile = argc > 1 ? argv[1] : "maps/default.map";
    if (!load_map(mapfile, &map)) {
        log_info("No map found, starting new map");
        for (int y = 0; y < MAP_HEIGHT; ++y)
            for (int x = 0; x < MAP_WIDTH; ++x)
                map.data[x][y] = 0;
    }

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        log_error("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }
    SDL_Window* window = SDL_CreateWindow("Map Editor", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIN_WIDTH, WIN_HEIGHT, 0);
    if (!window) {
        log_error("SDL_CreateWindow failed: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        log_error("SDL_CreateRenderer failed: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    bool running = true;
    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE) running = false;
                if (e.key.keysym.sym == SDLK_s) {
                    if (save_map(mapfile, &map))
                        log_info("Map saved to %s", mapfile);
                    else
                        log_error("Failed to save map");
                }
                if (e.key.keysym.sym == SDLK_1) current_tile = 1;
                if (e.key.keysym.sym == SDLK_2) current_tile = 2;
                if (e.key.keysym.sym == SDLK_3) current_tile = 3;
                if (e.key.keysym.sym == SDLK_4) current_tile = 4;
                if (e.key.keysym.sym == SDLK_0) current_tile = 0;
            }
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                int mx = e.button.x / CELL_SIZE;
                int my = e.button.y / CELL_SIZE;
                if (mx >= 0 && mx < MAP_WIDTH && my >= 0 && my < MAP_HEIGHT) {
                    map.data[mx][my] = current_tile;
                }
            }
        }
        SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
        SDL_RenderClear(renderer);
        draw_grid(renderer);
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}