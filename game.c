#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>

#include "game.h"
#include "render.h"
#include "board.h"
#include "tetromino.h"

void game_store_tetromino(Game* g) {
    board_store_tetromino(g->board, g->current);
    free(g->current);
    g->current = g->next;
    tetromino_move_to_spawn(g->current);
    g->next = tetromino_create(g, 1, BOARD_WIDTH-2);
}

Game* game_init() {
    Game* g = malloc(sizeof(Game));

    g->renderer = render_init();
    g->board = board_init();
    g->current = NULL;
    g->next = NULL;
    for (int i = 0; i < 7; i++) g->bag[i] = i;
    g->pulls = 0;
    g->running = true;
    g->levelled_up = true;
    g->score = 0;
    g->lines_cleared = 0;
    g->level = 1;
    g->last_block_tick_time = 0;

    for (int i = 0; i < MAX_ACTIONS; i++) {
        g->action[i] = 0;
        g->action_frames[i] = 0;
    }

    game_update_fall_delay(g);

    game_shuffle_bag(g);

    g->current = tetromino_create(g, 0, 0);
    // Check what we pulled, if we got S, Z or O, redo.
    while (g->current->shape == SHAPE_O
            || g->current->shape == SHAPE_S 
            || g->current->shape == SHAPE_Z) {
        free(g->current);
        game_shuffle_bag(g);
        g->current = tetromino_create(g, 0, 0);
    } 
    g->next = tetromino_create(g, 1, BOARD_WIDTH-2);

    return g;
}

void game_update_fall_delay(Game* g) {
    double d = pow((0.8 - ((g->level - 1) * 0.007)), (g->level - 1)) * 1000;
    g->fall_delay = (int)d;
}

void game_shutdown(Game* g) {
    free(g->current);
    free(g->next);
    free(g->board);
    render_shutdown(g->renderer);
    free(g->renderer);
    free(g);
}

void game_render(Game* g) {
    sprintf(g->renderer->score_text, "%d", g->score);

    render_clear_screen(g->renderer);
    render_grid(g->renderer);
    board_render(g->renderer, g->board);
    tetromino_render(g->renderer, g->current);
    tetromino_render(g->renderer, g->next);
    render_text(g->renderer, BLOCK_HEIGHT*5, BLOCK_WIDTH*11, g->renderer->score_text);
    render_text(g->renderer, 5, 5, g->renderer->framerate_text);
    render_border(g->renderer);
    render_update_screen(g->renderer);
}

void game_handle_input(Game* g) {
    for (int i = 0; i < MAX_ACTIONS; i++) {
        if (g->action[i] == 1) {
            switch (i) {
                case ACTION_LEFT:
                    if (tetromino_can_move_to(g->current, g->board, 0, -1, 0))
                        g->current->x -= 1;
                    break;
                case ACTION_RIGHT:
                    if (tetromino_can_move_to(g->current, g->board, 0, 1, 0))
                        g->current->x += 1;
                    break;
                case ACTION_DOWN:
                    if (tetromino_can_move_to(g->current, g->board, 1, 0, 0))
                        g->current->y += 1;
                    else
                        game_store_tetromino(g);
                    break;
                case ACTION_ROTATE:
                    if (tetromino_can_move_to(g->current, g->board, 0, 0, 1))
                        g->current->rotation = (g->current->rotation + 1) % 4;
                    break;
                case ACTION_HARDDROP:
                    while (tetromino_can_move_to(g->current, g->board, 1, 0, 0)) {
                        g->current->y += 1;
                    }
                    game_store_tetromino(g);
                    break;
            }
        }
        g->action[i] = 0;
    }
}

void game_update(Game* g) {
    /* Block falling - fall one cell every second. */
    if (g->renderer->frame_start_ms- g->last_block_tick_time >= g->fall_delay) {
        if (tetromino_can_move_to(g->current, g->board, 1, 0, 0)) 
            g->current->y += 1;
        else {
            game_store_tetromino(g);
        }
        g->last_block_tick_time = g->renderer->frame_start_ms;
    }

    board_update(g->board, g);

    //falltime = (0.8 - ((level-1) * 0.007)) ^ (level-1) [in seconds]
    if (g->lines_cleared % 10 == 0) {
        if (!g->levelled_up) {
            printf("level old: %d\n", g->level);
            g->level++;
            printf("level new: %d\n", g->level);
            printf("lines cleared: %d\n", g->lines_cleared);
            game_update_fall_delay(g);
        }
        g->levelled_up = true;
    } else {
        g->levelled_up = false;
    }
}

void game_shuffle_bag(Game* g) {
    g->pulls = 0;
    for (int i = 6; i > 0; i--) {
        int j = rand() % 7;
        int buf = g->bag[j];
        g->bag[j] = g->bag[i];
        g->bag[i] = buf;
    }
}

int game_pull_from_bag(Game* g) {
    g->pulls++;
    if (g->pulls > 6) {
        game_shuffle_bag(g);
    }
    return g->bag[g->pulls];
}

void game_clear_action_frames(Game* g) {
    for (int i = 0; i < MAX_ACTIONS; i++) {
        g->action_frames[i] = 0;
    }
}

void game_handle_action(Game* g, int action) {
    g->action_frames[action]++;
    switch (action) {
        case ACTION_LEFT:
        case ACTION_RIGHT:
        case ACTION_DOWN:
            if (g->action_frames[action] == 1 ||
                    (g->action_frames[action] > 10 &&
                     g->action_frames[action] % 3 == 0)) {
                g->action[action] = 1;
            }
            break;
        case ACTION_ROTATE:
        case ACTION_HARDDROP:
            if (g->action_frames[action] == 1) {
                g->action[action] = 1;
            }
            break;
    }
}
