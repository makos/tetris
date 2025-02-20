#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "render.h"
#include "defs.h"

Renderer* render_init() {
    SDL_Init(SDL_INIT_EVERYTHING);
    Renderer* r = malloc(sizeof(Renderer));

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

    return r;
}

SDL_Rect render_get_block_rect(int y, int x) {
    SDL_Rect r;

    r.y = y * BLOCK_HEIGHT;
    r.x = x * BLOCK_WIDTH;
    r.h = BLOCK_HEIGHT;
    r.w = BLOCK_WIDTH;

    return r;
}

void render_block(Renderer *r, int y, int x, int color) {
    SDL_Rect rect = render_get_block_rect(y, x);
    SDL_FillRect(r->w_surf, &rect, color);
}

void render_draw_border(Renderer *r) {

}

void render_shutdown(Renderer* r) { 
    TTF_CloseFont(r->font);
    TTF_Quit();

    SDL_DestroyWindow(r->w);
    r->w = NULL;
    r->w_surf = NULL;
    SDL_Quit();   
}

void render_clear_screen(Renderer* r) {
    SDL_FillRect(r->w_surf, NULL, COLOR_BLACK);
}

void render_update_screen(Renderer* r) {
    SDL_UpdateWindowSurface(r->w);
}

void render_text(Renderer* r, int y, int x, char* text) {
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

void render_start_frame(Renderer* r) {
    r->frame_start_ms = SDL_GetTicks64();
}

unsigned long render_update_delta(Renderer* r) {
    r->frame_end_ms = SDL_GetTicks64();
    return r->frame_end_ms - r->frame_start_ms;
}

void render_delay(Renderer* r) {
    render_update_delta(r);
    unsigned long delay = (r->frame_start_ms + 17) - r->frame_end_ms;
    if (delay > 0) {
        SDL_Delay(delay);
    }
}

void render_update_fps(Renderer* r) {
    r->frame_end_ms = SDL_GetTicks64();
    r->fps = 1000.0 / (r->frame_end_ms - r->frame_start_ms);
    sprintf(r->framerate_text, "%.0f", r->fps);
}

unsigned long render_get_ticks() {
    return SDL_GetTicks64();
}
