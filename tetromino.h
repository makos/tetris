typedef struct Renderer Renderer;
typedef struct Board Board;
typedef struct Game Game;

// Enum in order of shapes as they appear in shapes[][] below.
enum Shapes {SHAPE_L, SHAPE_I, SHAPE_O, SHAPE_J, SHAPE_S, SHAPE_Z, SHAPE_T};

typedef struct Tetromino {
    int shape;
    int rotation; // in range 0..3
    int x;
    int y;
} Tetromino;

Tetromino* tetromino_create(Game* g, int y, int x);
bool tetromino_bit_is_empty(Tetromino* t, int row, int col);
void tetromino_move_to_spawn(Tetromino *t);
void tetromino_render(Renderer *r, Tetromino *t);
bool tetromino_can_move_to(Tetromino* t, Board* b, int dest_y, int dest_x, int next_rot);
