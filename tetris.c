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
 * - hard drop
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
    SDL_Event e;
    render_update_fps(game->renderer);

    while (game->running) {
        render_start_frame(game->renderer);
        const Uint8* keys = SDL_GetKeyboardState(NULL);

        // SDL handling first
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                game->running = false;
            }
            if (keys[SDL_SCANCODE_ESCAPE]) {
                game->running = false;
            }
            if (keys[SDL_SCANCODE_LEFT]) {
                game->action[ACTION_LEFT] = 1;
            }
            if (keys[SDL_SCANCODE_RIGHT]) {
                game->action[ACTION_RIGHT] = 1;
            }
            if (keys[SDL_SCANCODE_DOWN]) {
                game->action[ACTION_DOWN] = 1;
            }
            if (keys[SDL_SCANCODE_Z] && !game->rotated && !e.key.repeat) {
                game->action[ACTION_ROTATE] = 1;
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
