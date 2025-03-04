#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <SDL3/SDL_pixels.h>

#include "tetromino.h"
#include "defs.h"
#include "render.h"
#include "board.h"
#include "game.h"

unsigned long colors[7][4] = {
 {0xef, 0x79, 0x21, 0xff}, // L
 {0x31, 0xc7, 0xef, 0xff}, // I
 {0xf7, 0xd3, 0x08, 0xff}, // O
 {0x5a, 0x65, 0xad, 0xff}, // J
 {0x42, 0xb6, 0x42, 0xff}, // S
 {0xef, 0x20, 0x29, 0xff}, // Z
 {0xad, 0x4d, 0x9c, 0xff}  // T
};

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
    { -4, 3 }, // L
    { -3, 3 }, // I
    { -4, 3 }, // O
    { -4, 2 }, // J
    { -4, 2 }, // S
    { -4, 3 }, // Z
    { -4, 2 }  // T
};

/* Create new randomly chosen piece. 
 * (y,x) - position to create the piece in; 0,0 to spawn on the board,
 * above BOARD_WIDTH to spawn on the side as a "next" piece.
 */
Tetromino* tetromino_create(Game* g, int y, int x) {
    int rand_shape = game_pull_from_bag(g);
    Tetromino* t = malloc(sizeof(Tetromino));

    t->shape = rand_shape;
    t->rotation = 0;
    t->y = y + start_offset[t->shape][0];
    t->x = x + start_offset[t->shape][1];
    t->color.r = colors[t->shape][0];
    t->color.g = colors[t->shape][1];
    t->color.b = colors[t->shape][2];
    t->color.a = colors[t->shape][3];

    return t;
}

Tetromino* tetromino_create_ghost(Tetromino* t, Game* g) {
    Tetromino* new_t = malloc(sizeof(Tetromino));
    memcpy(new_t, t, sizeof(Tetromino));

    new_t->color.a = 0x60;

    while (tetromino_can_move_to(new_t, g->board, 1, 0, 0)) {
        new_t->y++;
    }

    return new_t;
}

void tetromino_move_to_spawn(Tetromino* t) {
    t->y = start_offset[t->shape][0];
    t->x = start_offset[t->shape][1];
}

void tetromino_rotate(Tetromino* t, int dir) {
    t->rotation += dir;
}

void tetromino_move(Tetromino* t, int y, int x) {
    t->y += y;
    t->x += x;
}

/* Return true if a space in the 4x4 shape matrix is an empty (0) bit. */
bool tetromino_bit_is_empty(Tetromino* t, int row, int col) {
    int shift = col + row * 4;
    int bit = 0x8000; // 1000 0000 0000 0000
                      // shift it to the right by `shift` bits to check
                      // the given matrix value
    return !((bit >> shift) & shapes[t->shape][abs(t->rotation) % 4]);
}

/* Draw the given tetromino. */
void tetromino_render(Renderer* rend, Tetromino *t) {
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (!tetromino_bit_is_empty(t, y, x)) {
                render_block(rend, t->y + y, t->x + x, t->color);
            }
        }
    }
}

/* Check if current piece can move to specified (y,x,rotated) space.
 * "rotated" means that we should check if rotating the piece doesn't collide.
 * TODO: add wall kicks (check if we can rotate the piece by moving it one
 * cell to the side)
 */
bool tetromino_can_move_to(Tetromino* t, Board* b, int dest_y, int dest_x, int next_rot) { 
    // Save current rotation in case the checks fail.
    int old_rot = t->rotation;
    if (next_rot != 0) {
        tetromino_rotate(t, next_rot);
    }

    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (!(tetromino_bit_is_empty(t, y, x)) 
                && (t->x + x + dest_x < 0 
                    || t->x + x + dest_x > BOARD_WIDTH - 1 
                    || t->y + y + dest_y > BOARD_HEIGHT - 1)) {
                goto cant_move;
            }
            if ((!(tetromino_bit_is_empty(t, y, x)) && board_has_block(b, t->y + y + dest_y, t->x + x + dest_x))) {
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
