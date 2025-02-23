#include <math.h>

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

void render_border(Renderer* r) {
    SDL_Rect border = {10 * BLOCK_WIDTH, 0, 5, SCREEN_HEIGHT};
    SDL_FillRect(r->w_surf, &border, COLOR_BLUE);
}

double clamp(double d, double min, double max) { 
    const double t = d < min ? min : d;
    return t > max ? max : t;
}

void render_block(Renderer *rend, int y, int x, int color) {
    // Convert hex value to RGB, find max, add or substract from other colors,
    // convert again into hex. r,g,b are lighter colors, r2,g2,b2 are darker
    // used for shadows.
    Uint8 r, g, b;
    Uint8 r2, g2, b2;
    double maxval;

    SDL_GetRGB(color, rend->w_surf->format, &r, &g, &b);
    maxval = fmax(fmax(r, g), b);

    if (maxval == r) {
        r2 = r;
        g2 = clamp(g - 16, 0, 255);
        b2 = clamp(b - 16, 0, 255);
        g = clamp(g + 16, 0, 255);
        b = clamp(b + 16, 0, 255);
    } else if (maxval == g) {
        g2 = g;
        r2 = clamp(r - 16, 0, 255);
        b2 = clamp(b - 16, 0, 255);
        r = clamp(r + 16, 0, 255);
        b = clamp(b + 16, 0, 255);
    } else {
        b2 = b;
        r2 = clamp(r - 16, 0, 255);
        g2 = clamp(g - 16, 0, 255);
        r = clamp(r + 16, 0, 255);
        g = clamp(g + 16, 0, 255);
    }

    Uint32 color_light = SDL_MapRGB(rend->w_surf->format, r, g, b);
    Uint32 color_dim = SDL_MapRGB(rend->w_surf->format, r2, g2, b2);

    SDL_Rect rect = render_get_block_rect(y, x);
    // Borders - "shadows" on the outside of block.
    SDL_Rect bl = {rect.x, rect.y, 5, rect.h}; // border left
    SDL_Rect bu = {rect.x, rect.y, rect.w, 5}; // border up
    SDL_Rect br = {rect.x + rect.w - 5, rect.y, 5, rect.h}; // border right
    SDL_Rect bd = {rect.x, rect.y + rect.h - 5, rect.w, 5}; // border down
    SDL_FillRect(rend->w_surf, &rect, color);
    SDL_FillRect(rend->w_surf, &bl, color_light);
    SDL_FillRect(rend->w_surf, &bu, color_light);
    SDL_FillRect(rend->w_surf, &br, color_dim);
    SDL_FillRect(rend->w_surf, &bd, color_dim);
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

void render_grid(Renderer *r) {
    Uint32 grid_color = 0x101010;
    SDL_Rect vline = {0, 0, 1, SCREEN_HEIGHT};
    SDL_Rect hline = {0, 0, SCREEN_WIDTH - (BLOCK_WIDTH * 5), 1};

    for (int y = 0; y < BOARD_HEIGHT; y++) {
        SDL_FillRect(r->w_surf, &hline, grid_color);
        hline.y += BLOCK_HEIGHT;
    }

    for (int x = 0; x < BOARD_WIDTH; x++) {
        SDL_FillRect(r->w_surf, &vline, grid_color);
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
