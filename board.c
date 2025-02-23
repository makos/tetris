#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "tetromino.h"
#include "board.h"
#include "render.h"
#include "defs.h"
#include "game.h"

static int blink_frames = 0;
static int times_to_blink = 0;

/* Initialize the board to all empty spaces.
 */
Board* board_init() {
    Board* b = malloc(sizeof(Board));

    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            b->cell[y][x].type = BLOCK_EMPTY;
            b->cell[y][x].color = COLOR_BLACK;
            b->cell[y][x].to_clear = false;
            b->cell[y][x].show = false;
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
                b->cell[t->y + y][t->x + x].show = true;
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
    // Check bottom to top, left to right.
    for (int y = BOARD_HEIGHT - 1; y >= 0; y--) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (!board_has_block(b, y, x))
                break;
            if (x == BOARD_WIDTH - 1) {
                // Traverse the row again.
                for (int x2 = 0; x2 < BOARD_WIDTH; x2++) {
                    b->cell[y][x2].to_clear = true;
                }
                lines++;
            }
        }
    }
    return lines;
}

void board_clear_lines(Board* b) {
    int from_line = -1;
    int lines = 0;
    // Double buffer
    Board* copy = malloc(sizeof(Board));
    memcpy(copy, b, sizeof(Board));

    for (int y = BOARD_HEIGHT - 1; y >= 0; y--) {
        if (b->cell[y][0].to_clear) {
            lines++;
            if (from_line == -1) {
                from_line = y;
            }
        }
    }

    for (; from_line > 0; from_line--) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (from_line - lines < 0)
                lines = 1;
            b->cell[from_line][x] = b->cell[from_line-lines][x];
        }
    }

    free(copy);
}

/* Render blocks saved on the board.
 */
void board_render(Renderer* rend, Board* b) {
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (b->cell[y][x].show) {
                render_block(rend, y, x, b->cell[y][x].color);
            }
        }
    }
}

void board_update(Board* b, Game* g) {
    int lines = 0;
    // Animate blinking when clearing lines.
    if (times_to_blink > 0) {
        if (blink_frames++ > 10) {
            for (int y = 0; y < BOARD_HEIGHT; y++) {
                for (int x = 0; x < BOARD_WIDTH; x++) {
                    if (b->cell[y][x].to_clear && b->cell[y][x].show)
                        b->cell[y][x].show = false;
                    else if (b->cell[y][x].to_clear && !b->cell[y][x].show)
                        b->cell[y][x].show = true;
                }
            }
            times_to_blink--;
            blink_frames = 0;
        }
    } else {
        board_clear_lines(b);
        lines = board_check_lines(b);
    }

    if (lines == -1)
        g->running = false;
    else if (lines > 0) {
        g->score += 100 * lines * g->level;
        g->lines_cleared++;
        times_to_blink = 4;
    }
}
