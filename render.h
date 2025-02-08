typedef struct SDL_Window SDL_Window;
typedef struct SDL_Surface SDL_Surface;
typedef struct SDL_Rect SDL_Rect;

typedef struct Renderer {
    SDL_Window* w; // main game window
    SDL_Surface* w_surf;
} Renderer;

SDL_Rect get_block_rect(int y, int x);
void init_renderer(Renderer* r);
