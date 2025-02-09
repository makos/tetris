#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <time.h>

#include "tetromino.h"
#include "defs.h"
#include "render.h"
#include "board.h"

Renderer renderer;
//Block game_board[BOARD_HEIGHT][BOARD_WIDTH];
Board* game_board;
Tetromino* current_tetromino;
Tetromino* next_tetromino;

void shutdown_renderer() { 
    SDL_DestroyWindow(renderer.w);
    renderer.w = NULL;
    renderer.w_surf = NULL;
    SDL_Quit();   
}

void shutdown_game() {

}

void render() {
    SDL_FillRect(renderer.w_surf, NULL, COLOR_BLACK);
    render_board(&renderer, game_board);
    render_tetromino(current_tetromino, &renderer);
    render_tetromino(next_tetromino, &renderer);
    SDL_UpdateWindowSurface(renderer.w);
}

int main() {
    srand(time(NULL));
    bool running = true;
    Uint64 frame_start_time = 0;
    Uint64 frame_end_time = 0;
    Uint64 last_block_tick_time = 0;
    
    game_board = init_board();
    init_renderer(&renderer);

    current_tetromino = create_tetromino(0, 0);
    next_tetromino = create_tetromino(1, BOARD_WIDTH-2);

    SDL_Event e;
    while (running) {
        frame_start_time = SDL_GetTicks64();

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
                    if (can_move_to(current_tetromino, game_board, 0, -1, 0)) 
                        current_tetromino->x -= 1;
                    break;
                case SDLK_RIGHT:
                    if (can_move_to(current_tetromino, game_board, 0, 1, 0)) 
                        current_tetromino->x += 1;
                    break;
                case SDLK_DOWN:
                    if (can_move_to(current_tetromino, game_board, 1, 0, 0)) 
                        current_tetromino->y += 1;
                    else
                        goto store;
                    break;
                case SDLK_z:
                    if (can_move_to(current_tetromino, game_board, 0, 0, 1))
                        current_tetromino->rotation = (current_tetromino->rotation + 1) % 4;
                    break;
                }
            }
        }

        /* Block falling - fall one cell every second. */
        if (frame_start_time - last_block_tick_time >= BLOCK_TICK_INTERVAL) {
            if (can_move_to(current_tetromino, game_board, 1, 0, 0)) 
                current_tetromino->y += 1;
            else {
store:
                store_tetromino(game_board, current_tetromino);
                free(current_tetromino);
                current_tetromino = next_tetromino;
                move_to_spawn(current_tetromino);
                next_tetromino = create_tetromino(1, BOARD_WIDTH-2);
            }
            last_block_tick_time = frame_start_time;
        }

        if (!check_lines(game_board))
            running = false;
        
        render();

        frame_end_time = SDL_GetTicks64();
        int64_t delay = (frame_start_time + 16) - frame_end_time;
        if (delay > 0) {
            SDL_Delay(delay);            
        }
    }

    SDL_Delay(2000);
    free(current_tetromino);
    free(next_tetromino);
    free(game_board);
    shutdown_renderer();
    shutdown_game();
    return 0;
}
