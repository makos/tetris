#ifndef GAME_H
#define GAME_H

#define MAX_ACTIONS 5
#define MAX_TETROMINOS 7

typedef struct Renderer Renderer;
typedef struct Tetromino Tetromino;
typedef struct Board Board;

enum Actions { ACTION_LEFT, ACTION_RIGHT, ACTION_DOWN, ACTION_ROTATE, ACTION_HARDDROP };

typedef struct Game {
    Renderer* renderer;
    Board* board;
    Tetromino* current;
    Tetromino* next;

    int bag[MAX_TETROMINOS];
    int action[MAX_ACTIONS];
    int action_frames[MAX_ACTIONS];
    int pulls;
    bool running;
    bool levelled_up;
    int score;
    int lines_cleared;
    int fall_delay;
    int level;
    unsigned int block_frames;
} Game;

Game* game_init();

void game_update_fall_delay(Game* g);
void game_shutdown(Game* g);
void game_render(Game* g);
void game_update(Game* g);
void game_shuffle_bag(Game* g);
void game_handle_input(Game* g);
int game_pull_from_bag(Game* g);
void game_handle_action(Game* g, int action);
void game_clear_action_frames(Game* g);

#endif
