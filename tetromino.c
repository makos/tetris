#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#include <SDL2/SDL.h>

#include "tetromino.h"
#include "defs.h"
#include "render.h"
#include "board.h"

// Enum in order of shapes as they appear in shapes[][] below.
enum Shapes {SHAPE_L, SHAPE_I, SHAPE_O, SHAPE_J, SHAPE_S, SHAPE_Z, SHAPE_T};

/* Shapes and their rotations.
 * shapes[][] 2D array holds 7 tetrominos, each with 4 rotations.
 * They are encoded as a 3x4 matrix, each hex digit corresponds to one row
 * as such:
 * HEX     BINARY
 * 4       0100
 * 4       0100
 * 6       0110
 * 0       0000
 * Thus, the number 0x4460 forms an "L" shape.
 */
int shapes[7][4] = {
    { 0x0e80, 0xc440, 0x2e00, 0x4460 }, // L
    { 0x00f0, 0x2222, 0x00f0, 0x2222 }, // I
    { 0x0660, 0x0660, 0x0660, 0x0660 }, // O
    { 0x0710, 0x2260, 0x4700, 0x3220 }, // J
    { 0x0360, 0x2310, 0x0360, 0x2310 }, // S
    { 0x0c60, 0x4c80, 0x0c60, 0x4c80 }, // Z
    { 0x0720, 0x2620, 0x2700, 0x2320 }  // T
};

/* (y,x) offset for each tetromino shape, in order as above.
 * Used to spawn the pieces in correct positions.
 */
int start_offset[7][2] = {
    { -1, 3 }, // L
    { -2, 3 }, // I
    { -1, 3 }, // O
    { -1, 2 }, // J
    { -1, 2 }, // S
    { -1, 3 }, // Z
    { -1, 2 }  // T
};

/* Create new randomly chosen piece. 
 * (y,x) - position to create the piece in; 0,0 to spawn on the board,
 * above BOARD_WIDTH to spawn on the side as a "next" piece.
 */

Tetromino* create_tetromino(int y, int x) {
    int rand_shape = rand() % 7;
    Tetromino* t = malloc(sizeof(Tetromino));

    t->shape = rand_shape;
    //t->shape = SHAPE_T;
    t->rotation = 0;
    t->y = y + start_offset[t->shape][0];
    t->x = x + start_offset[t->shape][1];

    return t;
}

void move_to_spawn(Tetromino* t) {
    t->y = start_offset[t->shape][0];
    t->x = start_offset[t->shape][1];
}

/* Return true if a space in the 4x4 shape matrix is an empty (0) bit. */
bool is_empty(Tetromino* t, int row, int col) {
    int shift = col + row * 4;
    int bit = 0x8000; // 1000 0000 0000 0000
                      // shift it to the right by `shift` bits to check
                      // the given matrix value
    return !((bit >> shift) & shapes[t->shape][t->rotation]);
}

/* Draw the given tetromino. */
void render_tetromino (Tetromino *t, Renderer* rend) {
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (!is_empty(t, y, x)) {
                //TODO: Refactor this out into render.c and provide drawing methods.
                SDL_Rect r = get_block_rect(t->y + y, t->x + x);
                SDL_FillRect(rend->w_surf, &r, COLOR_WHITE);
            }
        }
    }
}

/* Check if current piece can move to specified (y,x,rotated) space.
 * "rotated" means that we should check if rotating the piece doesn't collide.
 * TODO: add collision with blocks stored in game_board.
 */
bool can_move_to(Tetromino* t, Board* b, int dest_y, int dest_x, int next_rot) { 
    // Save current rotation in case the checks fail.
    int old_rot = t->rotation % 4;
    if (next_rot) {
        t->rotation = (t->rotation + 1) % 4;
    }

    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (!(is_empty(t, y, x)) 
                && (t->x + x + dest_x < 0 
                    || t->x + x + dest_x > BOARD_WIDTH - 1 
                    || t->y + y + dest_y > BOARD_HEIGHT - 1)) {
                goto cant_move;
            }
            if ((!(is_empty(t, y, x)) && has_block(b, t->y + y + dest_y, t->x + x + dest_x))) {
                goto cant_move;
            }
        }
    }

    t->rotation = old_rot;
    return true;
cant_move:
    t->rotation = old_rot;
    return false;
}
