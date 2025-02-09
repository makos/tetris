#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "tetromino.h"
#include "board.h"
#include "render.h"
#include "defs.h"

/* Initialize the board to all empty spaces.
 */
Board* init_board() {
    Board* b = malloc(sizeof(Board));
    b->score = 0;
    b->lines_cleared = 0;
    b->level = 1;
    update_fall_delay(b);

    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            b->cell[y][x] = BLOCK_EMPTY;
        }
    }
    return b;
}

void update_fall_delay(Board* b) {
    double d = pow((0.8 - ((b->level - 1) * 0.007)), (b->level - 1)) * 1000;
    b->fall_delay = (int)d;
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

/* Return true if given (y,x) coordinate corresponds to a saved block in
 * the game board. 
 */
bool has_block(Board* b, int y, int x) {
    if (y < 0 || x < 0 || y > BOARD_HEIGHT - 1 || x > BOARD_WIDTH - 1)
        return false;

    if (b->cell[y][x] == BLOCK_EMPTY)
        return false;
    else
        return true;
}
/* Check if any lines need deleting (full), or if blocks reached the top of the
 * screen - which means game over.
 */
bool check_lines(Board* b) {
    for (int x = 0; x < BOARD_WIDTH; x++) {
        if (has_block(b, 0, x))
            return false;
    }
    // Check bottom to top, left to right
    for (int y = BOARD_HEIGHT - 1; y >= 0; y--) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (!has_block(b, y, x))
                break;
            if (x == BOARD_WIDTH - 1) {
                clear_line(b, y);
                b->score += 100;
                b->lines_cleared++;
            }
        }
    }
    return true;
}

void clear_line(Board* b, int y) {
    for (; y > 0; y--) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            b->cell[y][x] = b->cell[y-1][x];
        }
    }
}

/* Render blocks saved on the board.
 */
void render_board(Renderer* rend, Board* b) {
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            switch (b->cell[y][x]) {
                case BLOCK_EMPTY:
                    break;
                case BLOCK_FULL:
                    draw_block(rend, y, x, COLOR_RED);
                    break;
            }
        }
    }
}
