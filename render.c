#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
//#include <SDL3/SDL.h>

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

    if (TTF_Init() < 0) {
        printf("SDL TTF init error: %s\n", TTF_GetError());
    }

    r->font = TTF_OpenFont("FSEX302.ttf", 24);
    if (!r->font) {
        printf("Error opening font: %s\n", TTF_GetError());
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

void draw_block(Renderer *r, int y, int x, int color) {
    SDL_Rect rect = get_block_rect(y, x);
    SDL_FillRect(r->w_surf, &rect, color);
}

void draw_border(Renderer* r) {
}

void shutdown_renderer(Renderer* r) { 
    TTF_CloseFont(r->font);
    TTF_Quit();

    SDL_DestroyWindow(r->w);
    r->w = NULL;
    r->w_surf = NULL;
    SDL_Quit();   
}

void clear_screen(Renderer* r) {
    SDL_FillRect(r->w_surf, NULL, COLOR_BLACK);
}

void update_screen(Renderer* r) {
    SDL_UpdateWindowSurface(r->w);
}

void draw_text(Renderer* r, int y, int x, char* text) {
    SDL_Surface* surf_text;
    SDL_Color white = {255, 255, 255, 255};

    surf_text = TTF_RenderText_Solid(r->font, text, white);
    if (!surf_text) {
        printf("Failed to render text: %s\n", TTF_GetError());
    }

    SDL_Rect dest_rect = {x, y, surf_text->w, surf_text->h};

    SDL_BlitSurface(surf_text, NULL, r->w_surf, &dest_rect);

    SDL_FreeSurface(surf_text);
}
