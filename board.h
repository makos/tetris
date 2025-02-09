#ifndef __BOARD_H
#define __BOARD_H

#include "defs.h"

typedef struct Renderer Renderer;
typedef struct Tetromino Tetromino;

typedef enum BlockType {
    BLOCK_EMPTY,
    BLOCK_FULL
} BlockType;

typedef struct Block {
    //int x;
    //int y;
    BlockType type;
} Block;


void init_board(Block b[BOARD_HEIGHT][BOARD_WIDTH]);
void render_board(Renderer* rend, Block b[BOARD_HEIGHT][BOARD_WIDTH]);
void store_tetromino(Block b[BOARD_HEIGHT][BOARD_WIDTH], Tetromino* t);

#endif //__BOARD_H
