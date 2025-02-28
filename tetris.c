#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <time.h>

#include "game.h"
#include "tetromino.h"
#include "render.h"

/* TODO:
 * - better scoring (multipliers)
 * - sound (music and SFX)
 * - wall kicks
 * - ghost piece
 * - outline of stored blocks
 * - "lock" VFX (piece flashes)
 * - grace period after locking the piece
 * - make playfield 10x22 with top 2 rows hidden
 * BUG:
 * - game over should be when there's no place to spawn; now it spawns a piece
 *   and only after it gets stored it's game over
 */
int main() {
    srand(time(NULL));
    Game* game = game_init();
    if (!game) {
        printf("Error initializing game struct!\n");
        return -1;
    }
    SDL_Event e;
    render_update_fps(game->renderer);

    while (game->running) {
        render_start_frame(game->renderer);

        // SDL handling first
        // TODO: just make a function and pass the event to it, clear this mess up
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_EVENT_QUIT:
                    game->running = false;
                    break;
                case SDL_EVENT_KEY_UP:
                    game_clear_action_frames(game);
                    switch (game->state) {
                        case STATE_MENU:
                            switch (e.key.scancode) {
                                case SDL_SCANCODE_ESCAPE:
                                    game->running = false;
                                    break;
                                case SDL_SCANCODE_R:
                                    game_reset(game);
                                default:
                                    break;
                            }
                            break;
                        case STATE_GAME:
                            switch (e.key.scancode) {
                                case SDL_SCANCODE_ESCAPE:
                                    game->state = STATE_MENU;
                                    break;
                                default:
                                    break;
                            }
                            break;
                    } 
                }
            }

        const bool* keys = SDL_GetKeyboardState(NULL);

        //TODO: pass "keys" to a function in game.h and handle all this there
        if (game->state == STATE_GAME) {
            if (keys[SDL_SCANCODE_LEFT]) {
                game_handle_action(game, ACTION_LEFT);
            }
            if (keys[SDL_SCANCODE_RIGHT]) {
                game_handle_action(game, ACTION_RIGHT);
            }
            if (keys[SDL_SCANCODE_DOWN]) {
                game_handle_action(game, ACTION_DOWN);
            }
            if (keys[SDL_SCANCODE_X] || keys[SDL_SCANCODE_UP]) {
                game_handle_action(game, ACTION_ROTATE);
            }
            if (keys[SDL_SCANCODE_Z] || keys[SDL_SCANCODE_LCTRL] || keys[SDL_SCANCODE_RCTRL]) {
                game_handle_action(game, ACTION_ROTATE_CCW);
            }
            if (keys[SDL_SCANCODE_SPACE]) {
                game_handle_action(game, ACTION_HARDDROP);
            }
        } else if (game->state == STATE_MENU) {
            if (keys[SDL_SCANCODE_RETURN]) {
                game->state = STATE_GAME;
            }
        }

        game_handle_input(game);

        game_update(game);

        game_render(game);

        render_delay(game->renderer);
        render_update_fps(game->renderer);
    }
    printf("\nFinal score: %d\nLevel: %d\n", game->score, game->level);
    game_shutdown(game);
    return 0;
}
