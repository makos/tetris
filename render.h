#ifndef RENDER_H
#define RENDER_H

#include "color.h"

typedef struct SDL_Window SDL_Window;
typedef struct SDL_Surface SDL_Surface;
typedef struct SDL_FRect SDL_FRect;
typedef struct TTF_Font TTF_Font;
typedef struct SDL_Texture SDL_Texture;
typedef struct SDL_Renderer SDL_Renderer;

typedef struct Renderer {
    SDL_Window* w; // main game window
    TTF_Font* font;
    SDL_Texture* block;
    SDL_Renderer* rend;
    double fps;
    unsigned long frame_start_ms;
    unsigned long frame_end_ms;
} Renderer;

Renderer* render_init();
SDL_FRect render_get_block_rect(int y, int x);
void render_shutdown(Renderer* r);
void render_block(Renderer* r, int y, int x, Color color);
void render_clear_screen(Renderer* r);
void render_update_screen(Renderer* r);
void render_text(Renderer* r, int y, int x, char* text);
void render_border(Renderer *r);
void render_start_frame(Renderer* r);
unsigned long render_update_delta(Renderer* r);
void render_update_fps(Renderer* r);
void render_delay(Renderer* r);
unsigned long render_get_ticks();
void render_grid(Renderer* r);

#endif
