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
Board* game_board;
Tetromino* current_tetromino;
Tetromino* next_tetromino;

void shutdown_game() {
    free(current_tetromino);
    free(next_tetromino);
    free(game_board);
}

void render() {
    char score_text[256];
    sprintf(score_text, "%d", game_board->score);

    clear_screen(&renderer);
    render_board(&renderer, game_board);
    render_tetromino(&renderer, current_tetromino);
    render_tetromino(&renderer, next_tetromino);
    draw_text(&renderer, BLOCK_HEIGHT*5, BLOCK_WIDTH*11, score_text);
    update_screen(&renderer);
}

/* TODO:
 * - blocks with various colors
 * - scoring
 * - better spawning system - a randomized "bag"
 * - level progression (speed)
 * - restarting the game
 * - hard drop
 * - menu
 */
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
        if (frame_start_time - last_block_tick_time >= game_board->fall_delay) {
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

        //falltime = (0.8 - ((level-1) * 0.007)) ^ (level-1) [in seconds]
        if (game_board->lines_cleared % 10 == 0 && game_board->lines_cleared / 10 == game_board->level) {
            printf("level old: %d\n", game_board->level);
            game_board->level++;
            printf("level new: %d\n", game_board->level);
            update_fall_delay(game_board);
            printf("fall delay: %d\n", game_board->fall_delay);
        }
        render();

        frame_end_time = SDL_GetTicks64();
        int64_t delay = (frame_start_time + 16) - frame_end_time;
        if (delay > 0) {
            SDL_Delay(delay);            
        }
    }

    //TODO: hack, implement pausing and restarting the game.
    //SDL_Delay(2000);
    shutdown_renderer(&renderer);
    shutdown_game();
    return 0;
}
