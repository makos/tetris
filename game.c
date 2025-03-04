#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "game.h"
#include "render.h"
#include "board.h"
#include "tetromino.h"

#define LEFT -1
#define RIGHT 1
#define DOWN 1
#define ROTATE_CW 1
#define ROTATE_CCW -1
#define NEXT_MINO_OFFSET 4

void game_store_tetromino(Game* g) {
    board_store_tetromino(g->board, g->current);
    free(g->current);
    free(g->ghost);
    g->current = g->next;
    tetromino_move_to_spawn(g->current);
    g->next = tetromino_create(g, NEXT_MINO_OFFSET, BOARD_WIDTH-2);
    g->ghost = tetromino_create_ghost(g->current, g);
}

void game_zero(Game* g) {
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
    g->state = STATE_MENU;
    for (int i = 0; i < MAX_ACTIONS; i++) {
        g->action[i] = 0;
        g->action_frames[i] = 0;
    }
}

Game* game_init() {
    Game* g = malloc(sizeof(Game));

    g->renderer = render_init();
    if (!g->renderer) {
        return NULL;
    }
    game_zero(g);

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
    g->ghost = tetromino_create_ghost(g->current, g);
    g->next = tetromino_create(g, NEXT_MINO_OFFSET, BOARD_WIDTH-2);

    return g;
}

void game_reset(Game* g) {
    free(g->current);
    free(g->next);
    free(g->board);
    free(g->ghost);

    game_zero(g);
    g->state = STATE_GAME;

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
    g->ghost = tetromino_create_ghost(g->current, g);
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
    free(g->ghost);
    free(g->board);
    render_shutdown(g->renderer);
    free(g->renderer);
    free(g);
}

void game_render(Game* g) {
    char buf[256];
    memset(buf, 0, 256);

    render_clear_screen(g->renderer);

    switch (g->state) {
        case STATE_GAME:
            render_grid(g->renderer);
            board_render(g->renderer, g->board);
            tetromino_render(g->renderer, g->current);
            tetromino_render(g->renderer, g->next);
            tetromino_render(g->renderer, g->ghost);
            render_border(g->renderer);
            // Score
            sprintf(buf, "Score: %d", g->score);
            render_text(g->renderer, BLOCK_HEIGHT*5, BLOCK_WIDTH*11, buf);
            // Level
            sprintf(buf, "Level: %d", g->level);
            render_text(g->renderer, BLOCK_HEIGHT*6, BLOCK_WIDTH*11, buf);
            break;
        case STATE_MENU:
            sprintf(buf, "%s", "TETRIS");
            render_text(g->renderer, (SCREEN_HEIGHT - 24)/2, (SCREEN_WIDTH - 3)/2, buf);
            sprintf(buf, "%s", "PRESS ENTER TO PLAY");
            render_text(g->renderer, (SCREEN_HEIGHT + 24)/2, (SCREEN_WIDTH - 20)/2, buf);
            sprintf(buf, "%s", "ESCAPE TO QUIT");
            render_text(g->renderer, (SCREEN_HEIGHT + 60)/2, (SCREEN_WIDTH - 20)/2, buf);
            break;
    }
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
                    if (tetromino_can_move_to(g->current, g->board, 0, LEFT, 0)) {
                        tetromino_move(g->current, 0, LEFT);
                        tetromino_move(g->ghost, 0, LEFT);
                    }
                    break;
                case ACTION_RIGHT:
                    if (tetromino_can_move_to(g->current, g->board, 0, RIGHT, 0)) {
                        tetromino_move(g->current, 0, RIGHT);
                        tetromino_move(g->ghost, 0, RIGHT);
                    }
                    break;
                case ACTION_DOWN:
                    if (tetromino_can_move_to(g->current, g->board, DOWN, 0, 0))
                        tetromino_move(g->current, DOWN, 0);
                    else
                        game_store_tetromino(g);
                    break;
                case ACTION_ROTATE:
                    if (tetromino_can_move_to(g->current, g->board, 0, 0, ROTATE_CW))
                        tetromino_rotate(g->current, ROTATE_CW);
                    break;
                case ACTION_ROTATE_CCW:
                    if (tetromino_can_move_to(g->current, g->board, 0, 0, ROTATE_CCW)) {
                        tetromino_rotate(g->current, ROTATE_CCW);
                    }
                    break;
                case ACTION_HARDDROP:
                    while (tetromino_can_move_to(g->current, g->board, DOWN, 0, 0)) {
                        tetromino_move(g->current, DOWN, 0);
                    }
                    game_store_tetromino(g);
                    break;
            }
        }
        g->action[i] = 0;
    }
}

void game_update(Game* g) {
    if (g->state == STATE_GAME) {
        if (g->block_frames % g->fall_delay == 0) {
            if (tetromino_can_move_to(g->current, g->board, DOWN, 0, 0)) 
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

        g->ghost->rotation = g->current->rotation;
        g->ghost->y = g->current->y;
        while (tetromino_can_move_to(g->ghost, g->board, DOWN, 0, 0)) {
            g->ghost->y++;
        }
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
        case ACTION_ROTATE_CCW:
            if (g->action_frames[action] == 1) {
                g->action[action] = 1;
            }
            break;
    }
}
