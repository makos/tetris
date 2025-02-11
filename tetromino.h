typedef struct Renderer Renderer;
typedef struct Board Board;

typedef struct Tetromino {
    int shape;
    int rotation; // in range 0..3
    int x;
    int y;
} Tetromino;

Tetromino* create_tetromino(int y, int x);
bool is_empty(Tetromino* t, int row, int col);
void move_to_spawn(Tetromino *t);
void render_tetromino (Renderer *r, Tetromino *t);
bool can_move_to(Tetromino* t, Board* b, int dest_y, int dest_x, int next_rot);
