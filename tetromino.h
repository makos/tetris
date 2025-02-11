typedef struct Renderer Renderer;
typedef struct Board Board;

// Enum in order of shapes as they appear in shapes[][] below.
enum Shapes {SHAPE_L, SHAPE_I, SHAPE_O, SHAPE_J, SHAPE_S, SHAPE_Z, SHAPE_T};

typedef struct Tetromino {
    int shape;
    int rotation; // in range 0..3
    int x;
    int y;
} Tetromino;

Tetromino* create_tetromino(int y, int x);
bool is_empty(Tetromino* t, int row, int col);
void shuffle_bag();
int pull_from_bag();
void move_to_spawn(Tetromino *t);
void render_tetromino (Renderer *r, Tetromino *t);
bool can_move_to(Tetromino* t, Board* b, int dest_y, int dest_x, int next_rot);
