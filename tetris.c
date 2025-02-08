#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#define SCREEN_WIDTH 400
#define SCREEN_HEIGHT 800

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20

#define BLOCK_WIDTH SCREEN_WIDTH/BOARD_WIDTH
#define BLOCK_HEIGHT SCREEN_HEIGHT/BOARD_HEIGHT

#define BLOCK_TICK_INTERVAL 1000

#define COLOR_WHITE 0xffffff
#define COLOR_BLACK 0x000000

// Shapes and their rotations.
// They are encoded as a 4x4 matrix, each hex digit corresponds to one row
// as such:
// HEX     BINARY
// 4       0100
// 4       0100
// 6       0110
// 0       0000
// Thus, the number 0x4460 forms an "L" shape.
int tetromino_L[4] = { 0x4460, 0x02e0, 0x0622, 0x0740 };

typedef enum BlockType {
    BLOCK_EMPTY,
    BLOCK_FULL
} BlockType;

typedef struct Block {
    int x; // in game board space
    int y; // in game board space
    Uint32 color;
    SDL_Rect rect;
    BlockType type;
} Block;

typedef struct Tetromino {
    int shape;
    int x;
    int y;
} Tetromino;

SDL_Window* w; // main game window
SDL_Surface* w_surf;
SDL_Event e;
bool running = true;

Block game_board[BOARD_HEIGHT][BOARD_WIDTH];

void init_renderer() {
    SDL_Init(SDL_INIT_EVERYTHING);
    w = SDL_CreateWindow("Tetris",
                         SDL_WINDOWPOS_CENTERED,
                         SDL_WINDOWPOS_CENTERED,
                         SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (!w) {
        printf("SDL Error: %s\n", SDL_GetError());
    }
    
    w_surf = SDL_GetWindowSurface(w);
    if (!w_surf) {
        printf("SDL Surface error: %s\n", SDL_GetError());
    }
    /* SDL_FillRect(w_surf, NULL, SDL_MapRGB(w_surf->format, 0, 0, 0)); */
}

void init_board() {
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            game_board[y][x].rect.h = BLOCK_HEIGHT;
            game_board[y][x].rect.w = BLOCK_WIDTH;
            game_board[y][x].x = x;
            game_board[y][x].y = y;
            game_board[y][x].rect.x = x * BLOCK_WIDTH;
            game_board[y][x].rect.y = y * BLOCK_HEIGHT;
            game_board[y][x].type = BLOCK_EMPTY;
            game_board[y][x].color = COLOR_BLACK;
        }
    }
}

void render_board() {
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            switch (game_board[y][x].type) {
                case BLOCK_EMPTY:
                    SDL_FillRect(w_surf, &game_board[y][x].rect, COLOR_BLACK);
                    break;
                case BLOCK_FULL:
                    SDL_FillRect(w_surf, &game_board[y][x].rect, COLOR_WHITE);
                    break;
            }
        }
    }

}

void update_board(Tetromino* t) {
    int row = 0;
    int col = 0;
    for (int bit = 0x8000, i = 0; bit > 0 && i < 16; bit = bit >> 1, i++) {
        int ind_y = t->y + row;
        int ind_x = t->x + col;
        if (!(ind_y < 0 || ind_x < 0 || ind_y > BOARD_HEIGHT-1 || ind_x > BOARD_WIDTH-1)) {
            if (bit & t->shape) {
                game_board[ind_y][ind_x].type = BLOCK_FULL;
            } else {
                game_board[ind_y][ind_x].type = BLOCK_EMPTY;
            }

            if (col == 3) {
                row++;
                col = 0;
            } else {
                col++;
            }
        }
    }
}

void update_block(Uint64 dt, Tetromino* t, int move_x, int move_y) {
    int newpos_x = t->x + move_x;
    int newpos_y = t->y + move_y;
    Block board_buf[BOARD_HEIGHT][BOARD_WIDTH];

    // Check collision against screen boundaries.
    if (newpos_y < 0 || newpos_x < 0 || newpos_y + 3 > BOARD_HEIGHT-1 || newpos_x + 3 > BOARD_WIDTH-1) {
        return;
    }

    int row = 0;
    int col = 0;
    for (int bit = 0x8000, i = 0; bit > 0 && i < 16; bit = bit >> 1, i++) {
        if (bit & t->shape) {
            game_board[t->y + row][t->x + col].type = BLOCK_EMPTY;
            game_board[t->y + move_y + row][t->x + move_x + col].type = BLOCK_FULL;
        } else {
            game_board[ind_y][ind_x].type = BLOCK_EMPTY;
        }

        if (col == 3) {
            row++;
            col = 0;
        } else {
            col++;
        }
    }

    t->x = newpos_x;
    t->y = newpos_y;

    //update_board(t);

    /* Block falling - fall one cell every second. */
    //if (frame_start_time - last_block_tick_time >= BLOCK_TICK_INTERVAL) {
    //    //move_block(current_block, 0, -1);
    //    last_block_tick_time = frame_start_time;
    //}
}

void shutdown_renderer() { 
    SDL_DestroyWindow(w);
    w = NULL;
    w_surf = NULL;
    SDL_Quit();   
}

void shutdown_game() {

}


// Currently create L shaped tetromino at (1,1) coords;
// add random choice of tetromino and tweak spawn position
Tetromino* create_tetromino() {
    Tetromino* t = malloc(sizeof(Tetromino));

    t->shape = tetromino_L[0];
    t->x = 1;
    t->y = 1;

    return t;
}

void render() {
    //SDL_RenderClear(r);
    SDL_FillRect(w_surf, NULL, COLOR_BLACK);
    render_board();
    SDL_UpdateWindowSurface(w);
}

/* TODO:
   - Block collision with other blocks
   - Block shapes
   - Spawning new blocks
   - Refactor stuff out from main()
*/
int main() {
    Uint64 frame_start_time = 0;
    Uint64 frame_end_time = 0;
    Uint64 delta = 0;
    Uint64 last_block_tick_time = 0;
    
    init_board();
    init_renderer();

    int movedir_x = 0;
    int movedir_y = 0;
    int falldir_x = 0;
    int falldir_y = 0;
    
    Tetromino* current_tetromino = create_tetromino();
    int rot = 1;

    while (running) {
        frame_start_time = SDL_GetTicks64();
        delta = frame_start_time - frame_end_time;

        movedir_x = 0;
        movedir_y = 0;
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                running = false;
            }
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                case SDLK_ESCAPE:
                    running = false;
                    break;
                case SDLK_LEFT:
                    movedir_x = -1;
                    break;
                case SDLK_RIGHT:
                    movedir_x = 1;
                    break;
                case SDLK_DOWN:
                    movedir_y = 1;
                    break;
                case SDLK_UP:
                    movedir_y = -1;
                    break;
                case 'z':
                    current_tetromino->shape = tetromino_L[rot++ % 4];
                }
            }
        }

        update_block(delta, current_tetromino, movedir_x, movedir_y);
        render();

        /* Block falling - fall one cell every second. */
        //if (frame_start_time - last_block_tick_time >= BLOCK_TICK_INTERVAL) {
        //    //move_block(current_block, 0, -1);
        //    last_block_tick_time = frame_start_time;
        //}

        frame_end_time = SDL_GetTicks64();
        int64_t delay = (frame_start_time + 16) - frame_end_time;
        if (delay > 0) {
            SDL_Delay(delay);            
        }
    }

    free(current_tetromino);
    shutdown_renderer();
    shutdown_game();
    return 0;
}
