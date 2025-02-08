#include <SDL2/SDL.h>

#include "board.h"
#include "render.h"
#include "defs.h"

void init_board(Block b[BOARD_HEIGHT][BOARD_WIDTH]) {
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            b[y][x].x = x;
            b[y][x].y = y;
            b[y][x].type = BLOCK_EMPTY;
        }
    }
}

void render_board(Renderer* rend, Block b[BOARD_HEIGHT][BOARD_WIDTH]) {
    SDL_Rect r;
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            r = get_block_rect(y, x);
            switch (b[y][x].type) {
                case BLOCK_EMPTY:
                    SDL_FillRect(rend->w_surf, &r, COLOR_BLACK);
                    break;
                case BLOCK_FULL:
                    SDL_FillRect(rend->w_surf, &r, COLOR_WHITE);
                    break;
            }
        }
    }
}
