#ifndef _CHESS_
#define _CHESS_

#define NTYPES 7
#define NTEAMS 2
#define NPIECES 32
#define TEAMSIZE 16
#define BCOLS 8
#define BROWS 8
#define CS_ISIZE 8

/* DATA STRUCTURES */
// TODO label me
typedef struct {
    int col;
    int row;
} coord;

typedef struct {
    coord** data;
    int iter;
    int size;
} coord_stack;

typedef enum{
    REGULAR,
    Q_CASTLE,
    K_CASTLE,
    PAWN_STEP,
    PAWN_CAPTURE,
    PAWN_JUMP,
    NONE,
} PATH_TYPE;

typedef struct{
    PATH_TYPE path_type;
    coord* coords;
    int num_coords;
} path;

typedef struct{
    int num_paths;
    path* paths;
} moves_list;

typedef enum {
    W_PAWN,
    B_PAWN,
    ROOK,
    KNIGHT,
    BISHOP,
    KING,
    QUEEN
} TYPE;

typedef enum {
    WHITE,
    BLACK
} COLOR;

typedef struct{
    coord pos;
    COLOR color;
    TYPE type;
    int index;
    // state vars
    int nMoves;
    int captured;
    int jump;
} piece;

typedef struct {
    piece* primary;
    piece* secondary;   // captured piece OR rook in castle
    TYPE primary_type;
    coord start;
    coord end;
    PATH_TYPE pathType;
    int enPassant;
} log_node;

typedef struct {
    log_node* data;
    int iter;
    int size;
} log;

extern moves_list moves[NTYPES];
extern piece roster[NTEAMS][TEAMSIZE];
extern piece* board[BCOLS][BROWS];
extern log move_log; 
extern wchar_t piece_chars[NTEAMS][NTYPES];
extern char *color_strings[NTEAMS];
extern char *piece_strings[NTYPES];
extern int enPassantFlag;

/* FUNCTION PROTOTYPES */
// initializations
void initialize_game();

// coords
coord* newCoord(int col, int row);
coord* copyCoord(coord* c);
void setCoord(int col, int row, coord* cur);
void addCoords(coord* sum, coord* add);
void updateCoord(coord *old, coord *new);
int compareCoords(coord* a, coord* b);

// coord stack
coord_stack* coord_stack_init();
void coord_stack_push(coord_stack*, coord*);
int coord_stack_search(coord_stack*, coord*);
void coord_stack_free(coord_stack* cs);

// move
int move(piece* p, coord* c);
int is_valid_move(piece* p, coord* c);
int is_attack(piece* p, coord* c);
PATH_TYPE find_move(piece* p, coord* c);
PATH_TYPE find_move_no_castle(piece* p, coord* c);
coord_stack* get_moves(piece*);
int traversePath(path* pa, piece* p);
int is_pawn_capture(piece* p, coord* c);
int is_pawn_jump(piece* p, coord* c);
int is_pawn_step(piece* p, coord* c);
int is_k_castle(piece* p, coord* c);
int is_q_castle(piece* p, coord* c);
void enPassant(piece* p, coord* c);
void q_castle(piece* p, coord* c);
void k_castle(piece* p, coord* c);
void promote(piece* p);
void clearJumps(COLOR co);
void pushLog(piece* primary, piece* secondary, coord start, coord end, PATH_TYPE path, int enPassant);
log_node popLog();
void undoMove();
coord_stack* find_intermediate_squares(piece* p, coord* c);
int can_move(piece* p);

// board
int isInbounds(coord* c);
int isEmptySpace(coord* c);
piece* getSquare(coord* c);
int setSquareAndPiece(coord* c, piece* p);
coord_stack* get_square_reachers(coord* c, COLOR co);
int is_check(COLOR co);
coord_stack* get_checkers(COLOR co);
int is_mate(COLOR co);
int is_stale(COLOR co);

// print-outs
void print_coord_stack(coord_stack*);
void print_moves_list();
void print_roster();
void print_board();
void print_captured(COLOR co);

#endif //_CHESS_
