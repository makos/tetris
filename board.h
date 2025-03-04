#ifndef BOARD_H
#define BOARD_H

#include "defs.h"
#include "color.h"

typedef struct Renderer Renderer;
typedef struct Tetromino Tetromino;
typedef struct Game Game;

typedef enum BlockType { BLOCK_EMPTY, BLOCK_FULL } BlockType;

typedef struct Cell {
    BlockType type;
    //unsigned long color;
    Color color;
    int to_clear;
    int show;
} Cell;

typedef struct Board {
    Cell cell[BOARD_HEIGHT][BOARD_WIDTH];
} Board;


Board* board_init();
bool board_has_block(Board* b, int y, int x);
void board_render(Renderer* rend, Board* b); 
void board_store_tetromino(Board* b, Tetromino* t);
int board_check_lines(Board* b);
void board_clear_lines(Board* b);
void board_update(Board* b, Game* g);

#endif
