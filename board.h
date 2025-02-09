#ifndef __BOARD_H
#define __BOARD_H

#include "defs.h"

typedef struct Renderer Renderer;
typedef struct Tetromino Tetromino;

typedef enum BlockType {
    BLOCK_EMPTY,
    BLOCK_FULL
} BlockType;

typedef struct Board {
    BlockType cell[BOARD_HEIGHT][BOARD_WIDTH];
    int score;
    int lines_cleared;
    int fall_delay;
    int level;
} Board;


Board* init_board();
bool has_block(Board* b, int y, int x);
void render_board(Renderer* rend, Board* b); 
void store_tetromino(Board* b, Tetromino* t);
bool check_lines(Board* b);
void clear_line(Board* b, int y);
void update_fall_delay(Board* b);

#endif //__BOARD_H
