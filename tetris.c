#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <time.h>

#include "tetromino.h"
#include "defs.h"
#include "render.h"
#include "board.h"

//TODO: Game struct that holds these?? Do we need it for a small game like this?
Renderer renderer;
Board* game_board;
Tetromino* current_tetromino;
Tetromino* next_tetromino;
double framerate = 0;

enum Actions { ACTION_LEFT, ACTION_RIGHT, ACTION_DOWN, ACTION_ROTATE };

void shutdown_game() {
    free(current_tetromino);
    free(next_tetromino);
    free(game_board);
}

void render() {
    char score_text[256];
    sprintf(score_text, "%d", game_board->score);
    char framerate_text[32];
    sprintf(framerate_text, "%.0f", framerate);

    clear_screen(&renderer);
    render_board(&renderer, game_board);
    render_tetromino(&renderer, current_tetromino);
    render_tetromino(&renderer, next_tetromino);
    draw_text(&renderer, BLOCK_HEIGHT*5, BLOCK_WIDTH*11, score_text);
    draw_text(&renderer, 5, 5, framerate_text);
    update_screen(&renderer);
}

/* TODO:
 * - blocks with various colors
 * - better scoring (multipliers)
 * - restarting the game
 * - hard drop
 * - wall kicks
 * - menu
 * - update to SDL3?
 * - refactor the mess
 * - being able to move the block while rotating - SDL GetKeyboardState or sth
 */
int main() {
    srand(time(NULL));
    bool running = true;
    bool levelled_up = true;
    Uint64 frame_start_time = 0;
    Uint64 frame_end_time = 0;
    Uint64 last_block_tick_time = 0;
    Uint64 rotated_time = 0;
    
    game_board = init_board();
    init_renderer(&renderer);
    shuffle_bag();

    current_tetromino = create_tetromino(0, 0);
    // Check what we pulled, if we got S, Z or O, redo.
    while (current_tetromino->shape == SHAPE_O
            || current_tetromino->shape == SHAPE_S 
            || current_tetromino->shape == SHAPE_Z) {
        free(current_tetromino);
        shuffle_bag();
        current_tetromino = create_tetromino(0, 0);
    } 
    next_tetromino = create_tetromino(1, BOARD_WIDTH-2);

    SDL_Event e;
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    int action[] = { 0, 0, 0, 0 };
    int action_arr_size = sizeof(action) / sizeof(action[0]);
    bool rotated = false;

    while (running) {
        frame_start_time = SDL_GetTicks64();

        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                running = false;
            }
            if (keys[SDL_SCANCODE_ESCAPE]) {
                running = false;
            }
            if (keys[SDL_SCANCODE_LEFT]) {
                action[ACTION_LEFT] = 1;
            }
            if (keys[SDL_SCANCODE_RIGHT]) {
                action[ACTION_RIGHT] = 1;
            }
            if (keys[SDL_SCANCODE_DOWN]) {
                action[ACTION_DOWN] = 1;
            }
            if (keys[SDL_SCANCODE_Z] && !rotated && !e.key.repeat) {
                action[ACTION_ROTATE] = 1;
            }
        }
        // Process actions - TODO move into a function.
        for (int i = 0; i < action_arr_size; i++) {
            if (action[i]) {
                switch (i) {
                    case ACTION_LEFT:
                        //printf("left\n");
                        if (can_move_to(current_tetromino, game_board, 0, -1, 0))
                            current_tetromino->x -= 1;
                        break;
                    case ACTION_RIGHT:
                        //printf("right\n");
                        if (can_move_to(current_tetromino, game_board, 0, 1, 0))
                            current_tetromino->x += 1;
                        break;
                    case ACTION_DOWN:
                        //printf("down\n");
                        if (can_move_to(current_tetromino, game_board, 1, 0, 0))
                            current_tetromino->y += 1;
                        else
                            goto store;
                        break;
                    case ACTION_ROTATE:
                        rotated = true;
                        rotated_time = SDL_GetTicks64();
                        if (can_move_to(current_tetromino, game_board, 0, 0, 1))
                            current_tetromino->rotation = (current_tetromino->rotation + 1) % 4;
                        break;
                }
            }
            action[i] = 0;
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
        if (game_board->lines_cleared % 10 == 0) {
            if (!levelled_up) {
                printf("level old: %d\n", game_board->level);
                game_board->level++;
                printf("level new: %d\n", game_board->level);
                printf("lines cleared: %d\n", game_board->lines_cleared);
                update_fall_delay(game_board);
            }
            levelled_up = true;
        } else {
            levelled_up = false;
        }
        render();

        frame_end_time = SDL_GetTicks64();
        // Don't allow rotations for 5 frames since last one (16*5=80).
        if (frame_start_time - rotated_time > 80) {
            rotated = false;
        }

        // Delay if we can, to achieve 60 FPS (16,(6) ms).
        int64_t delay = (frame_start_time + 17) - frame_end_time;
        if (delay > 0) {
            SDL_Delay(delay);            
        }
        // FPS counter.
        frame_end_time = SDL_GetTicks64();
        framerate = 1000.0 / (frame_end_time - frame_start_time);
    }

    //TODO: hack, implement pausing and restarting the game.
    //SDL_Delay(2000);
    printf("\nFinal score: %d\nLevel: %d\n", game_board->score, game_board->level);
    shutdown_renderer(&renderer);
    shutdown_game();
    return 0;
}
