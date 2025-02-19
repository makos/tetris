#ifndef RENDER_H
#define RENDER_H

typedef struct SDL_Window SDL_Window;
typedef struct SDL_Surface SDL_Surface;
typedef struct SDL_Rect SDL_Rect;
typedef struct TTF_Font TTF_Font;

typedef struct Renderer {
    SDL_Window* w; // main game window
    SDL_Surface* w_surf;
    TTF_Font* font;
    char score_text[256];
    char framerate_text[32];
    double fps;
    unsigned long frame_start_ms;
    unsigned long frame_end_ms;
} Renderer;

Renderer* render_init();
SDL_Rect render_get_block_rect(int y, int x);
void render_shutdown(Renderer* r);
void render_block(Renderer* r, int y, int x, int color);
void render_clear_screen(Renderer* r);
void render_update_screen(Renderer* r);
void render_text(Renderer* r, int y, int x, char* text);
void render_draw_border(Renderer *r);
void render_start_frame(Renderer* r);
unsigned long render_update_delta(Renderer* r);
void render_update_fps(Renderer* r);
void render_delay(Renderer* r);
unsigned long render_get_ticks();

#endif
