#include <SDL2/SDL.h>

#include "render.h"
#include "defs.h"

void init_renderer(Renderer* r) {
    SDL_Init(SDL_INIT_EVERYTHING);
    r->w = SDL_CreateWindow("Tetris",
                         SDL_WINDOWPOS_CENTERED,
                         SDL_WINDOWPOS_CENTERED,
                         SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (!r->w) {
        printf("SDL Error: %s\n", SDL_GetError());
    }
    
    r->w_surf = SDL_GetWindowSurface(r->w);
    if (!r->w_surf) {
        printf("SDL Surface error: %s\n", SDL_GetError());
    }
}

SDL_Rect get_block_rect(int y, int x) {
    SDL_Rect r;

    r.y = y * BLOCK_HEIGHT;
    r.x = x * BLOCK_WIDTH;
    r.h = BLOCK_HEIGHT;
    r.w = BLOCK_WIDTH;

    return r;
}
