#include <SDL2/SDL.h>
#include <stdbool.h>

#include "tetromino.h"
#include "board.h"
#include "render.h"
#include "defs.h"

/* Initialize the board to all empty spaces.
 */
void init_board(Block b[BOARD_HEIGHT][BOARD_WIDTH]) {
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            //b[y][x].x = x;
            //b[y][x].y = y;
            b[y][x].type = BLOCK_EMPTY;
        }
    }
}

/* Save the piece to the board. */
void store_tetromino(Block b[BOARD_HEIGHT][BOARD_WIDTH], Tetromino* t) {
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (!is_empty(t, y, x)) {
                b[t->y + y][t->x + x].type = BLOCK_FULL;
            }
        }
    }
}

/* Render blocks saved on the board.
 * TODO: move the SDL calls out into API methods from render.c.
 */
void render_board(Renderer* rend, Block b[BOARD_HEIGHT][BOARD_WIDTH]) {
    SDL_Rect r;
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            r = get_block_rect(y, x);
            switch (b[y][x].type) {
                case BLOCK_EMPTY:
                    // e.g. draw_block(color);
                    SDL_FillRect(rend->w_surf, &r, COLOR_BLACK);
                    break;
                case BLOCK_FULL:
                    SDL_FillRect(rend->w_surf, &r, COLOR_WHITE);
                    break;
            }
        }
    }
}
