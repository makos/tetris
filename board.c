#include <stdlib.h>
#include <stdbool.h>

#include "tetromino.h"
#include "board.h"
#include "render.h"
#include "defs.h"

/* Initialize the board to all empty spaces.
 */
Board* board_init() {
    Board* b = malloc(sizeof(Board));

    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            b->cell[y][x].type = BLOCK_EMPTY;
            b->cell[y][x].color = COLOR_BLACK;
        }
    }
    return b;
}

/* Save the piece to the board. */
void board_store_tetromino(Board* b, Tetromino* t) {
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (!tetromino_bit_is_empty(t, y, x)) {
                b->cell[t->y + y][t->x + x].type = BLOCK_FULL;
                b->cell[t->y + y][t->x + x].color = t->color;
            }
        }
    }
}

/* Return true if given (y,x) coordinate corresponds to a saved block in
 * the game board. 
 */
bool board_has_block(Board* b, int y, int x) {
    if (y < 0 || x < 0 || y > BOARD_HEIGHT - 1 || x > BOARD_WIDTH - 1)
        return false;

    if (b->cell[y][x].type == BLOCK_EMPTY)
        return false;
    else
        return true;
}
/* Check if any lines need deleting (full), or if blocks reached the top of the
 * screen - which means game over. Returns number of lines cleared, or -1 if
 * blocks reached top of screen - meaning game over.
 */
int board_check_lines(Board* b) {
    for (int x = 0; x < BOARD_WIDTH; x++) {
        if (board_has_block(b, 0, x))
            return -1; // GAME OVER
    }
    int lines = 0;
    // Check bottom to top, left to right
    for (int y = BOARD_HEIGHT - 1; y >= 0; y--) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (!board_has_block(b, y, x))
                break;
            if (x == BOARD_WIDTH - 1) {
                board_clear_line(b, y);
                lines++;
            }
        }
    }
    return lines;
}

void board_clear_line(Board* b, int y) {
    for (; y > 0; y--) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            b->cell[y][x] = b->cell[y-1][x];
        }
    }
}

/* Render blocks saved on the board.
 */
void board_render(Renderer* rend, Board* b) {
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            switch (b->cell[y][x].type) {
                case BLOCK_EMPTY:
                    break;
                case BLOCK_FULL:
                    render_block(rend, y, x, b->cell[y][x].color);
                    break;
            }
        }
    }
}
