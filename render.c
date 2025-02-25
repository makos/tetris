#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include "render.h"
#include "defs.h"

SDL_Texture* load_texture(Renderer* r, char* path) {
    SDL_Surface *s = IMG_Load(path);
    SDL_Texture* t = SDL_CreateTextureFromSurface(r->rend, s);
    SDL_FreeSurface(s);
    return t;
}

Renderer* render_init() {
    SDL_Init(SDL_INIT_EVERYTHING);
    Renderer* r = malloc(sizeof(Renderer));

    r->w = SDL_CreateWindow("Tetris",
                         SDL_WINDOWPOS_CENTERED,
                         SDL_WINDOWPOS_CENTERED,
                         SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (!r->w) {
        printf("SDL Error: %s\n", SDL_GetError());
        return NULL;
    }
    
    r->rend = SDL_CreateRenderer(r->w, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!r->rend) {
        printf("SDL Renderer error: %s\n", SDL_GetError());
        return NULL;
    }

    SDL_SetRenderDrawColor(r->rend, 0x0, 0x0, 0x0, 0xff);
    IMG_Init(IMG_INIT_PNG);

    if (TTF_Init() < 0) {
        printf("SDL TTF init error: %s\n", TTF_GetError());
        return NULL;
    }

    r->font = TTF_OpenFont("FSEX302.ttf", 24);
    if (!r->font) {
        printf("Error opening font: %s\n", TTF_GetError());
        return NULL;
    }

    r->block = load_texture(r, "res/block.png");

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

void render_border(Renderer* r) {
    SDL_Rect border = {10 * BLOCK_WIDTH, 0, 5, SCREEN_HEIGHT};
    SDL_SetRenderDrawColor(r->rend, 0x01, 0x80, 0xff, 0xff);
    SDL_RenderFillRect(r->rend, &border);
}

double clamp(double d, double min, double max) { 
    const double t = d < min ? min : d;
    return t > max ? max : t;
}

void render_block(Renderer *rend, int y, int x, int color) {
    Uint8 r, g, b;
    SDL_Surface* w = SDL_GetWindowSurface(rend->w);
    SDL_GetRGB(color, w->format, &r, &g, &b);

    SDL_Rect rect = render_get_block_rect(y, x);
    SDL_SetTextureColorMod(rend->block, r, g, b);
    SDL_RenderCopy(rend->rend, rend->block, NULL, &rect);
}

void render_shutdown(Renderer* r) { 
    TTF_CloseFont(r->font);
    TTF_Quit();

    SDL_DestroyTexture(r->block);

    SDL_DestroyRenderer(r->rend);
    SDL_DestroyWindow(r->w);

    IMG_Quit();
    SDL_Quit();   
}

void render_clear_screen(Renderer* r) {
    SDL_SetRenderDrawColor(r->rend, 0x0, 0x0, 0x0, 0xff);
    SDL_RenderClear(r->rend);
}

void render_update_screen(Renderer* r) {
    SDL_RenderPresent(r->rend);
}

void render_grid(Renderer *r) {
    SDL_Rect vline = {0, 0, 1, SCREEN_HEIGHT};
    SDL_Rect hline = {0, 0, SCREEN_WIDTH - (BLOCK_WIDTH * 5), 1};

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

    surf_text = TTF_RenderText_Solid(r->font, text, white);
    if (!surf_text) {
        printf("Failed to render text: %s\n", TTF_GetError());
    }
    SDL_Rect dest_rect = {x, y, surf_text->w, surf_text->h};

    SDL_Texture* text_t = SDL_CreateTextureFromSurface(r->rend, surf_text);
    SDL_RenderCopy(r->rend, text_t, NULL, &dest_rect);

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
    long delay = (r->frame_start_ms + 17) - r->frame_end_ms;
    if (delay > 0) {
        SDL_Delay(delay);
    }
}

void render_update_fps(Renderer* r) {
    r->frame_end_ms = SDL_GetTicks64();
    r->fps = 1000.0 / (r->frame_end_ms - r->frame_start_ms);
}

unsigned long render_get_ticks() {
    return SDL_GetTicks64();
}
