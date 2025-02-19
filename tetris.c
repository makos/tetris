#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <time.h>

#include "game.h"
#include "tetromino.h"
#include "render.h"

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
 * - add "timer" header and implement separate timers (for falling, FPS etc.)
 */
int main() {
    srand(time(NULL));
    Game* game = game_init();
    SDL_Event e;
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    render_update_fps(game->renderer);

    while (game->running) {
        render_start_frame(game->renderer);

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

    //TODO: hack, implement pausing and restarting the game.
    //SDL_Delay(2000);
    //printf("\nFinal score: %d\nLevel: %d\n", game_board->score, game_board->level);
    //shutdown_renderer(&renderer);
    game_shutdown(game);
    return 0;
}
