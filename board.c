#include <SDL2/SDL.h>
#include <stdbool.h>

#include "tetromino.h"
#include "board.h"
#include "render.h"
#include "defs.h"

/* Initialize the board to all empty spaces.
 */
Board* init_board() {
    Board* b = malloc(sizeof(Board));

    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            b->cell[y][x] = BLOCK_EMPTY;
        }
    }
    return b;
}

/* Save the piece to the board. */
void store_tetromino(Board* b, Tetromino* t) {
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (!is_empty(t, y, x)) {
                b->cell[t->y + y][t->x + x] = BLOCK_FULL;
            }
        }
    }
}

bool has_block(Board* b, int y, int x) {
    if (y < 0 || x < 0 || y > BOARD_HEIGHT - 1 || x > BOARD_WIDTH - 1)
        return false;

    if (b->cell[y][x] == BLOCK_EMPTY)
        return false;
    else
        return true;
}

/* Render blocks saved on the board.
 * TODO: move the SDL calls out into API methods from render.c.
 */
void render_board(Renderer* rend, Board* b) {
    SDL_Rect r;
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            r = get_block_rect(y, x);
            switch (b->cell[y][x]) {
                case BLOCK_EMPTY:
                    // e.g. draw_block(color);
                    //SDL_FillRect(rend->w_surf, &r, COLOR_BLACK);
                    break;
                case BLOCK_FULL:
                    SDL_FillRect(rend->w_surf, &r, COLOR_RED);
                    break;
            }
        }
    }
}
