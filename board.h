#ifndef BOARD_H
#define BOARD_H

#include "defs.h"

typedef struct Renderer Renderer;
typedef struct Tetromino Tetromino;

typedef enum BlockType { BLOCK_EMPTY, BLOCK_FULL } BlockType;

typedef struct Cell {
    BlockType type;
    unsigned long color;
} Cell;

typedef struct Board {
    Cell cell[BOARD_HEIGHT][BOARD_WIDTH];
} Board;


Board* board_init();
bool board_has_block(Board* b, int y, int x);
void board_render(Renderer* rend, Board* b); 
void board_store_tetromino(Board* b, Tetromino* t);
int board_check_lines(Board* b);
void board_clear_line(Board* b, int y);

#endif
