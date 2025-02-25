#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <time.h>

#include "game.h"
#include "tetromino.h"
#include "render.h"

/* TODO:
 * - better scoring (multipliers)
 * - sound (music and SFX)
 * - restarting the game
 * - wall kicks
 * - menu
 * - update to SDL3?
 * BUG:
 * - game over should be when there's no place to spawn; now it spawns a piece
 *   and only after it gets stored it's game over
 */
int main() {
    srand(time(NULL));
    Game* game = game_init();
    if (!game) {
        printf("Error!\n");
        return -1;
    }
    SDL_Event e;
    render_update_fps(game->renderer);

    while (game->running) {
        render_start_frame(game->renderer);

        // SDL handling first
        while (SDL_PollEvent(&e) != 0) {
            switch (e.type) {
                case SDL_QUIT:
                    game->running = false;
                    break;
                case SDL_KEYUP:
                    game_clear_action_frames(game);
                    break;
            }
        }

        const Uint8* keys = SDL_GetKeyboardState(NULL);

        if (keys[SDL_SCANCODE_ESCAPE]) {
            game->running = false;
            break;
        }
        if (keys[SDL_SCANCODE_LEFT]) {
            game_handle_action(game, ACTION_LEFT);
        }
        if (keys[SDL_SCANCODE_RIGHT]) {
            game_handle_action(game, ACTION_RIGHT);
        }
        if (keys[SDL_SCANCODE_DOWN]) {
            game_handle_action(game, ACTION_DOWN);
        }
        if (keys[SDL_SCANCODE_Z]) {
            game_handle_action(game, ACTION_ROTATE);
        }
        if (keys[SDL_SCANCODE_SPACE]) {
            game_handle_action(game, ACTION_HARDDROP);
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
