typedef struct Renderer Renderer;
typedef struct Board Board;

typedef struct Tetromino {
    int shape;
    int rotation; // in range 0..3
    int x;
    int y;
} Tetromino;

Tetromino* create_tetromino();
bool is_empty(Tetromino* t, int row, int col);
void render_tetromino (Tetromino *t, Renderer *r);
bool can_move_to(Tetromino* t, Board* b, int dest_y, int dest_x, int next_rot);
