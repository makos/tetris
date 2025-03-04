#include <stdlib.h>
#include <stdio.h>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_image/SDL_image.h>

#include "render.h"
#include "defs.h"

SDL_Texture* load_texture(Renderer* r, char* path) {
    SDL_Texture* t = IMG_LoadTexture(r->rend, path);
    SDL_SetTextureBlendMode(t, SDL_BLENDMODE_BLEND);
    return t;
}

Renderer* render_init() {
    SDL_Init(SDL_INIT_VIDEO);
    Renderer* r = malloc(sizeof(Renderer));

    SDL_CreateWindowAndRenderer("Tetris", SCREEN_WIDTH, SCREEN_HEIGHT, 0, &r->w, &r->rend);
    if (!r->w) {
        printf("SDL Error: %s\n", SDL_GetError());
        return NULL;
    }
    
    if (!r->rend) {
        printf("SDL Renderer error: %s\n", SDL_GetError());
        return NULL;
    }

    SDL_SetRenderDrawColor(r->rend, 0x0, 0x0, 0x0, 0xff);
    SDL_SetRenderDrawBlendMode(r->rend, SDL_BLENDMODE_BLEND);

    if (!TTF_Init()) {
        printf("SDL TTF init error: %s\n", SDL_GetError());
        return NULL;
    }

    r->font = TTF_OpenFont("FSEX302.ttf", 24);
    if (!r->font) {
        printf("Error opening font: %s\n", SDL_GetError());
        return NULL;
    }

    r->block = load_texture(r, "res/block.png");

    return r;
}

SDL_FRect render_get_block_rect(int y, int x) {
    SDL_FRect r;

    r.y = y * BLOCK_HEIGHT;
    r.x = x * BLOCK_WIDTH;
    r.h = BLOCK_HEIGHT;
    r.w = BLOCK_WIDTH;

    return r;
}

void render_border(Renderer* r) {
    SDL_FRect border = {10 * BLOCK_WIDTH, 0, 5, SCREEN_HEIGHT};
    SDL_SetRenderDrawColor(r->rend, 0x01, 0x80, 0xff, 0xff);
    SDL_RenderFillRect(r->rend, &border);
}

double clamp(double d, double min, double max) { 
    const double t = d < min ? min : d;
    return t > max ? max : t;
}

void render_block(Renderer *rend, int y, int x, Color color) {
    SDL_FRect rect = render_get_block_rect(y, x);
    SDL_SetTextureColorMod(rend->block, color.r, color.g, color.b);
    SDL_SetTextureAlphaMod(rend->block, color.a);
    SDL_RenderTexture(rend->rend, rend->block, NULL, &rect);
}

void render_shutdown(Renderer* r) { 
    TTF_CloseFont(r->font);
    TTF_Quit();

    SDL_DestroyTexture(r->block);

    SDL_DestroyRenderer(r->rend);
    SDL_DestroyWindow(r->w);

    SDL_Quit();   
}

void render_clear_screen(Renderer* r) {
    SDL_SetRenderDrawColor(r->rend, 0, 0, 0, 0xff);
    SDL_RenderClear(r->rend);
}

void render_update_screen(Renderer* r) {
    SDL_RenderPresent(r->rend);
}

void render_grid(Renderer *r) {
    SDL_FRect vline = {0, 0, 1, SCREEN_HEIGHT};
    SDL_FRect hline = {0, 0, SCREEN_WIDTH - (BLOCK_WIDTH * 5), 1};

    SDL_SetRenderDrawColor(r->rend, 0x10, 0x10, 0x10, 0xff);

    for (int y = 0; y < BOARD_HEIGHT; y++) {
        SDL_RenderFillRect(r->rend, &hline);
        hline.y += BLOCK_HEIGHT;
    }

    for (int x = 0; x < BOARD_WIDTH; x++) {
        SDL_RenderFillRect(r->rend, &vline);
        vline.x += BLOCK_WIDTH;
    }
}

void render_text(Renderer* r, int y, int x, char* text) {
    SDL_Surface* surf_text;
    SDL_Color white = {255, 255, 255, 255};

    surf_text = TTF_RenderText_Solid(r->font, text, 0, white);
    if (!surf_text) {
        printf("Failed to render text: %s\n", SDL_GetError());
    }
    SDL_FRect dest_rect = {x, y, surf_text->w, surf_text->h};

    SDL_Texture* text_t = SDL_CreateTextureFromSurface(r->rend, surf_text);
    SDL_RenderTexture(r->rend, text_t, NULL, &dest_rect);

    SDL_DestroySurface(surf_text);
}

void render_start_frame(Renderer* r) {
    r->frame_start_ms = SDL_GetTicks();
}

unsigned long render_update_delta(Renderer* r) {
    r->frame_end_ms = SDL_GetTicks();
    return r->frame_end_ms - r->frame_start_ms;
}

void render_delay(Renderer* r) {
    render_update_delta(r);
    long delay = (r->frame_start_ms + 17) - r->frame_end_ms;
    if (delay > 0) {
        SDL_Delay(delay);
    }
}

void render_update_fps(Renderer* r) {
    r->frame_end_ms = SDL_GetTicks();
    r->fps = 1000.0 / (r->frame_end_ms - r->frame_start_ms);
}

unsigned long render_get_ticks() {
    return SDL_GetTicks();
}
