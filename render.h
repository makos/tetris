typedef struct SDL_Window SDL_Window;
typedef struct SDL_Surface SDL_Surface;
typedef struct SDL_Rect SDL_Rect;
typedef struct TTF_Font TTF_Font;

typedef struct Renderer {
    SDL_Window* w; // main game window
    SDL_Surface* w_surf;
    TTF_Font* font;
} Renderer;

SDL_Rect get_block_rect(int y, int x);
void init_renderer(Renderer* r);
void shutdown_renderer(Renderer* r);
void draw_block(Renderer* r, int y, int x, int color);
void clear_screen(Renderer* r);
void update_screen(Renderer* r);
void draw_text(Renderer* r, int y, int x, char* text);
