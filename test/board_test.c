#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>

#include "../board.h"

typedef struct Renderer Renderer;
typedef struct Tetromino {
    int type;
    int color;
    bool show;
} Tetromino;

void render_block(Renderer* rend, int y, int x, int color) {
}

void tetromino_bit_is_empty(Tetromino *t, int row, int col) {
}

int main() {
    Board *b = board_init();
    SDL_Surface* surf;
    SDL_PixelFormat* fmt;
    SDL_Color* color;

    surf = IMG_Load("test1_in.png");
    fmt = surf->format;

    if (fmt->BitsPerPixel != 8) {
        printf("Error, not an 8-bit image!\n");
        return -1;
    }

    SDL_LockSurface(surf);

    Uint8* pixel = (Uint8 *)surf->pixels;
    for (int y = 0; y < surf->h; y++) {
        for (int x = 0; x < surf->w; x++) {
            Uint8 ind = pixel[fmt->BytesPerPixel * (y * surf->pitch + x)];
            color = &fmt->palette->colors[ind];
            if (color->r == 0xff) {
                printf("White pixel at (%d, %d)\n", x, y);
            }
        }
    }

    SDL_UnlockSurface(surf);

    SDL_FreeSurface(surf);

    SDL_Quit();
    free(b);
    return 0;
}


