#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#include "tetromino.h"
#include "defs.h"
#include "render.h"
#include "board.h"

Renderer renderer;
Block game_board[BOARD_HEIGHT][BOARD_WIDTH];
bool running = true;

void shutdown_renderer() { 
    SDL_DestroyWindow(renderer.w);
    renderer.w = NULL;
    renderer.w_surf = NULL;
    SDL_Quit();   
}

void shutdown_game() {

}

void render(Tetromino* t) {
    SDL_FillRect(renderer.w_surf, NULL, COLOR_BLACK);
    render_board(&renderer, game_board);
    render_tetromino(t, &renderer);
    SDL_UpdateWindowSurface(renderer.w);
}

int main() {
    Uint64 frame_start_time = 0;
    Uint64 frame_end_time = 0;
    Uint64 last_block_tick_time = 0;
    
    init_board(game_board);
    init_renderer(&renderer);

    Tetromino* current_tetromino = create_tetromino();

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
                    if (can_move_to(current_tetromino, 0, -1, 0)) 
                        current_tetromino->x -= 1;
                    break;
                case SDLK_RIGHT:
                    if (can_move_to(current_tetromino, 0, 1, 0)) 
                        current_tetromino->x += 1;
                    break;
                case SDLK_DOWN:
                    if (can_move_to(current_tetromino, 1, 0, 0)) 
                        current_tetromino->y += 1;
                    break;
                case 'z':
                    if (can_move_to(current_tetromino, 0, 0, 1)) {
                        current_tetromino->rotation = (current_tetromino->rotation + 1) % 4;
                    }
                }
            }
        }


        /* Block falling - fall one cell every second. */
        if (frame_start_time - last_block_tick_time >= BLOCK_TICK_INTERVAL) {
            if (can_move_to(current_tetromino, 1, 0, 0)) 
                current_tetromino->y += 1;
            last_block_tick_time = frame_start_time;
        }
        
        render(current_tetromino);

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
