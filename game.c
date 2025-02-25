#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

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
    if (!g->renderer) {
        return NULL;
    }
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
    g->block_frames = 0;

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
    // falltime = (0.8 - ((level-1) * 0.007)) ^ (level-1) [in seconds]
    // we convert it to frames per second.
    double d = pow((0.8 - ((g->level - 1) * 0.007)), (g->level - 1)) * 1000;
    d = d / 16.667;
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
    char buf[256];
    memset(buf, 0, 256);

    render_clear_screen(g->renderer);
    render_grid(g->renderer);
    board_render(g->renderer, g->board);
    tetromino_render(g->renderer, g->current);
    tetromino_render(g->renderer, g->next);
    render_border(g->renderer);
    // Score
    sprintf(buf, "Score: %d", g->score);
    render_text(g->renderer, BLOCK_HEIGHT*5, BLOCK_WIDTH*11, buf);
    // Level
    sprintf(buf, "Level: %d", g->level);
    render_text(g->renderer, BLOCK_HEIGHT*6, BLOCK_WIDTH*11, buf);
    // FPS
    sprintf(buf, "%.2f", g->renderer->fps);
    render_text(g->renderer, 5, 5, buf);
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
    if (g->block_frames % g->fall_delay == 0) {
        if (tetromino_can_move_to(g->current, g->board, 1, 0, 0)) 
            g->current->y += 1;
        else {
            game_store_tetromino(g);
        }
    }

    board_update(g->board, g);

    if (g->lines_cleared % 10 == 0) {
        if (!g->levelled_up) {
            g->level++;
            game_update_fall_delay(g);
        }
        g->levelled_up = true;
    } else {
        g->levelled_up = false;
    }

    g->block_frames++;
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
