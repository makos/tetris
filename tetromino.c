#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#include <SDL2/SDL.h>

#include "tetromino.h"
#include "defs.h"
#include "render.h"

enum {SHAPE_L, SHAPE_I, SHAPE_O, SHAPE_J, SHAPE_S, SHAPE_Z, SHAPE_T};

// Shapes and their rotations.
// shapes[][] 2D array holds 7 tetrominos, each with 4 rotations.
// They are encoded as a 4x4 matrix, each hex digit corresponds to one row
// as such:
// HEX     BINARY
// 4       0100
// 4       0100
// 6       0110
// 0       0000
// Thus, the number 0x4460 forms an "L" shape.
int shapes[7][4] = {
    { 0x0e80, 0xc440, 0x2e00, 0x4460 }, // L
    { 0x00f0, 0x2222, 0x00f0, 0x2222 }, // I
    { 0x0660, 0x0660, 0x0660, 0x0660 }, // O
    { 0x0710, 0x2260, 0x4700, 0x3220 }, // J
    { 0x0360, 0x2310, 0x0360, 0x2310 }, // S
    { 0x0c60, 0x4c80, 0x0c60, 0x4c80 }, // Z
    { 0x0720, 0x2620, 0x2700, 0x2320 }  // T
};

Tetromino* create_tetromino() {
    Tetromino* t = malloc(sizeof(Tetromino));

    t->shape = SHAPE_T;
    t->rotation = 0;
    t->x = 1;
    t->y = 1;

    return t;
}

bool is_empty(Tetromino* t, int row, int col) {
    int shift = col + row * 4;
    int bit = 0x8000;
    return !((bit >> shift) & shapes[t->shape][t->rotation]);
}

void render_tetromino (Tetromino *t, Renderer* rend) {
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (!is_empty(t, y, x)) {
                SDL_Rect r = get_block_rect(t->y + y, t->x + x);
                SDL_FillRect(rend->w_surf, &r, COLOR_WHITE);
            }
        }
    }
}

bool can_move_to(Tetromino* t, int dest_y, int dest_x, int next_rot) { 
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
                t->rotation = old_rot;
                return false;
            }
        }
    }
    t->rotation = old_rot;
    return true;
}
