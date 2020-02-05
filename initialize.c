#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include "chess.h"

moves_list moves[NTYPES];
piece roster[NTEAMS][TEAMSIZE];
piece* board[BCOLS][BROWS];
log move_log;
wchar_t piece_chars[NTEAMS][NTYPES];
char *color_strings[NTEAMS] = {"White", "Black"};
char *piece_strings[NTYPES] = {"Pawn[w]", "Pawn[b]", "Rook", "Knight", 
    "Bishop", "King", "Queen"};
int enPassantFlag;

void initialize_moves_list();
void initialize_roster();
void initialize_board();
void initialize_move_log();
void initialize_piece_chars();
void initialize_piece(piece* cur, COLOR c, TYPE t, int index, int col, int row);

void initialize_game(){
    initialize_moves_list();
    initialize_roster();
    initialize_board();
    initialize_move_log();
    initialize_piece_chars();
    enPassantFlag = 0;
}

void initialize_moves_list(){
    moves_list* cur_ml;

    // W_PAWN
    cur_ml = &moves[W_PAWN];
    cur_ml->num_paths = 4;
    cur_ml->paths = malloc(sizeof(path)*cur_ml->num_paths);
    for(int i = 0; i < cur_ml->num_paths; ++i){
        cur_ml->paths[i].coords = malloc(sizeof(coord));
        cur_ml->paths[i].num_coords = 1;
    }
    cur_ml->paths[0].path_type = PAWN_STEP;
    cur_ml->paths[1].path_type = PAWN_CAPTURE;
    cur_ml->paths[2].path_type = PAWN_CAPTURE;
    cur_ml->paths[3].path_type = PAWN_JUMP;
    setCoord(0, 1, &cur_ml->paths[0].coords[0]);
    setCoord(1,1, &cur_ml->paths[1].coords[0]);
    setCoord(-1,1, &cur_ml->paths[2].coords[0]);
    setCoord(0,2, &cur_ml->paths[3].coords[0]);

    // B_PAWN
    cur_ml = &moves[B_PAWN];
    cur_ml->num_paths = 4;
    cur_ml->paths = malloc(sizeof(path)*4);
    for(int i = 0; i < 4; ++i){
        cur_ml->paths[i].coords = malloc(sizeof(coord));
        cur_ml->paths[i].num_coords = 1;
    }
    cur_ml->paths[0].path_type = PAWN_STEP;
    cur_ml->paths[1].path_type = PAWN_CAPTURE;
    cur_ml->paths[2].path_type = PAWN_CAPTURE;
    cur_ml->paths[3].path_type = PAWN_JUMP;
    setCoord(0, -1, &cur_ml->paths[0].coords[0]);
    setCoord(1, -1, &cur_ml->paths[1].coords[0]);
    setCoord(-1, -1, &cur_ml->paths[2].coords[0]);
    setCoord(0,-2, &cur_ml->paths[3].coords[0]);

    // ROOK
    cur_ml = &moves[ROOK];
    cur_ml->num_paths = 4;
    cur_ml->paths = malloc(sizeof(path)*4);
    for(int i = 0; i < 4; ++i){
        cur_ml->paths[i].coords = malloc(sizeof(coord)*7);
        cur_ml->paths[i].num_coords = 7;
    }
    cur_ml->paths[0].path_type = REGULAR;
    cur_ml->paths[1].path_type = REGULAR;
    cur_ml->paths[2].path_type = REGULAR;
    cur_ml->paths[3].path_type = REGULAR;
    for(int j, i = 0; i < 7; ++i){
        j = i + 1;
        setCoord(0, j, &cur_ml->paths[0].coords[i]);
        setCoord(j, 0, &cur_ml->paths[1].coords[i]);
        setCoord(0, -j, &cur_ml->paths[2].coords[i]);
        setCoord(-j, 0, &cur_ml->paths[3].coords[i]);
    }

    // KNIGHT
    cur_ml = &moves[KNIGHT];
    cur_ml->num_paths = 8;
    cur_ml->paths = malloc(sizeof(path)*8);
    for(int i = 0; i < 8; ++i){
        cur_ml->paths[i].coords = malloc(sizeof(coord));
        cur_ml->paths[i].num_coords = 1;
    }
    cur_ml->paths[0].path_type = REGULAR;
    cur_ml->paths[1].path_type = REGULAR;
    cur_ml->paths[2].path_type = REGULAR;
    cur_ml->paths[3].path_type = REGULAR;
    cur_ml->paths[4].path_type = REGULAR;
    cur_ml->paths[5].path_type = REGULAR;
    cur_ml->paths[6].path_type = REGULAR;
    cur_ml->paths[7].path_type = REGULAR;
    setCoord(1,2, &cur_ml->paths[0].coords[0]);
    setCoord(2,1, &cur_ml->paths[1].coords[0]);
    setCoord(2,-1, &cur_ml->paths[2].coords[0]);
    setCoord(1,-2, &cur_ml->paths[3].coords[0]);
    setCoord(-1,-2, &cur_ml->paths[4].coords[0]);
    setCoord(-2,-1, &cur_ml->paths[5].coords[0]);
    setCoord(-2,1, &cur_ml->paths[6].coords[0]);
    setCoord(-1,2, &cur_ml->paths[7].coords[0]);

    // BISHOP
    cur_ml = &moves[BISHOP];
    cur_ml->num_paths = 4;
    cur_ml->paths = malloc(sizeof(path)*4);
    for(int i = 0; i < 4; ++i){
        cur_ml->paths[i].coords = malloc(sizeof(coord)*7);
        cur_ml->paths[i].num_coords = 7;
    }
    cur_ml->paths[0].path_type = REGULAR;
    cur_ml->paths[1].path_type = REGULAR;
    cur_ml->paths[2].path_type = REGULAR;
    cur_ml->paths[3].path_type = REGULAR;
    for(int j, i = 0; i < 7; ++i){
        j = i + 1;
        setCoord(j, j, &cur_ml->paths[0].coords[i]);
        setCoord(j, -j, &cur_ml->paths[1].coords[i]);
        setCoord(-j, -j, &cur_ml->paths[2].coords[i]);
        setCoord(-j, j, &cur_ml->paths[3].coords[i]);
    }

    // QUEEN
    cur_ml = &moves[QUEEN];
    cur_ml->num_paths = 8;
    cur_ml->paths = malloc(sizeof(path)*8);
    for(int i = 0; i < 8; ++i){
        cur_ml->paths[i].coords = malloc(sizeof(coord)*7);
        cur_ml->paths[i].num_coords = 7;
    }
    cur_ml->paths[0].path_type = REGULAR;
    cur_ml->paths[1].path_type = REGULAR;
    cur_ml->paths[2].path_type = REGULAR;
    cur_ml->paths[3].path_type = REGULAR;
    cur_ml->paths[4].path_type = REGULAR;
    cur_ml->paths[5].path_type = REGULAR;
    cur_ml->paths[6].path_type = REGULAR;
    cur_ml->paths[7].path_type = REGULAR;
    for(int j, i = 0; i < 7; ++i){
        j = i + 1;
        setCoord(0, j, &cur_ml->paths[0].coords[i]);
        setCoord(j, 0, &cur_ml->paths[1].coords[i]);
        setCoord(0, -j, &cur_ml->paths[2].coords[i]);
        setCoord(-j, 0, &cur_ml->paths[3].coords[i]);
        setCoord(j, j, &cur_ml->paths[4].coords[i]);
        setCoord(j, -j, &cur_ml->paths[5].coords[i]);
        setCoord(-j, -j, &cur_ml->paths[6].coords[i]);
        setCoord(-j, j, &cur_ml->paths[7].coords[i]);
    }

    // KING
    cur_ml = &moves[KING];
    cur_ml->num_paths = 10;
    cur_ml->paths = malloc(sizeof(path)*10);
    for(int i = 0; i < 10; ++i){
        cur_ml->paths[i].coords = malloc(sizeof(coord));
        cur_ml->paths[i].num_coords = 1;
    }
    cur_ml->paths[0].path_type = REGULAR;
    cur_ml->paths[1].path_type = REGULAR;
    cur_ml->paths[2].path_type = REGULAR;
    cur_ml->paths[3].path_type = REGULAR;
    cur_ml->paths[4].path_type = REGULAR;
    cur_ml->paths[5].path_type = REGULAR;
    cur_ml->paths[6].path_type = REGULAR;
    cur_ml->paths[7].path_type = REGULAR;
    setCoord(1, 1, &cur_ml->paths[0].coords[0]);
    setCoord(1, 0, &cur_ml->paths[1].coords[0]);
    setCoord(1, -1, &cur_ml->paths[2].coords[0]);
    setCoord(0, -1, &cur_ml->paths[3].coords[0]);
    setCoord(-1, -1, &cur_ml->paths[4].coords[0]);
    setCoord(-1, 0, &cur_ml->paths[5].coords[0]);
    setCoord(-1, 1, &cur_ml->paths[6].coords[0]);
    setCoord(0, 1, &cur_ml->paths[7].coords[0]);
    cur_ml->paths[8].path_type = Q_CASTLE;
    cur_ml->paths[9].path_type = K_CASTLE;
    setCoord(-2, 0, &cur_ml->paths[8].coords[0]);
    setCoord(2, 0, &cur_ml->paths[9].coords[0]);
}

void initialize_roster(){

    initialize_piece(&roster[WHITE][0], WHITE, ROOK, 0, 0, 0);
    initialize_piece(&roster[WHITE][1], WHITE, KNIGHT, 1, 1, 0);
    initialize_piece(&roster[WHITE][2], WHITE, BISHOP, 2, 2, 0);
    initialize_piece(&roster[WHITE][3], WHITE, QUEEN, 3, 3, 0);
    initialize_piece(&roster[WHITE][4], WHITE, KING, 4, 4, 0);
    initialize_piece(&roster[WHITE][5], WHITE, BISHOP, 5, 5, 0);
    initialize_piece(&roster[WHITE][6], WHITE, KNIGHT, 6, 6, 0);
    initialize_piece(&roster[WHITE][7], WHITE, ROOK, 7, 7, 0);
    initialize_piece(&roster[WHITE][8], WHITE, W_PAWN, 8, 0, 1);
    initialize_piece(&roster[WHITE][9], WHITE, W_PAWN, 9, 1, 1);
    initialize_piece(&roster[WHITE][10], WHITE, W_PAWN, 10, 2, 1);
    initialize_piece(&roster[WHITE][11], WHITE, W_PAWN, 11, 3, 1);
    initialize_piece(&roster[WHITE][12], WHITE, W_PAWN, 12, 4, 1);
    initialize_piece(&roster[WHITE][13], WHITE, W_PAWN, 13, 5, 1);
    initialize_piece(&roster[WHITE][14], WHITE, W_PAWN, 14, 6, 1);
    initialize_piece(&roster[WHITE][15], WHITE, W_PAWN, 15, 7, 1);

    initialize_piece(&roster[BLACK][0], BLACK, ROOK, 0, 0, 7);
    initialize_piece(&roster[BLACK][1], BLACK, KNIGHT, 1, 1, 7);
    initialize_piece(&roster[BLACK][2], BLACK, BISHOP, 2, 2, 7);
    initialize_piece(&roster[BLACK][3], BLACK, QUEEN, 3, 3, 7);
    initialize_piece(&roster[BLACK][4], BLACK, KING, 4, 4, 7);
    initialize_piece(&roster[BLACK][5], BLACK, BISHOP, 5, 5, 7);
    initialize_piece(&roster[BLACK][6], BLACK, KNIGHT, 6, 6, 7);
    initialize_piece(&roster[BLACK][7], BLACK, ROOK, 7, 7, 7);
    initialize_piece(&roster[BLACK][8], BLACK, B_PAWN, 8, 0, 6);
    initialize_piece(&roster[BLACK][9], BLACK, B_PAWN, 9, 1, 6);
    initialize_piece(&roster[BLACK][10], BLACK, B_PAWN, 10, 2, 6);
    initialize_piece(&roster[BLACK][11], BLACK, B_PAWN, 11, 3, 6);
    initialize_piece(&roster[BLACK][12], BLACK, B_PAWN, 12, 4, 6);
    initialize_piece(&roster[BLACK][13], BLACK, B_PAWN, 13, 5, 6);
    initialize_piece(&roster[BLACK][14], BLACK, B_PAWN, 14, 6, 6);
    initialize_piece(&roster[BLACK][15], BLACK, B_PAWN, 15, 7, 6);

}

void initialize_piece(piece* cur, COLOR c, TYPE t, int index, int col, int row){
    cur->color = c;
    cur->type = t;
    cur->index = index;
    cur->nMoves = 0;
    cur->captured = 0;
    setCoord(col, row, &cur->pos);
}

void initialize_move_log(){
    move_log.iter = 0;
    move_log.size = 8;
    move_log.data = malloc(move_log.size*sizeof(log_node));
}

void initialize_piece_chars(){
    piece_chars[WHITE][W_PAWN] = 0x2659;
    piece_chars[WHITE][ROOK] = 0x2656;
    piece_chars[WHITE][KNIGHT] = 0x2658;
    piece_chars[WHITE][BISHOP] = 0x2657;
    piece_chars[WHITE][KING] = 0x2654;
    piece_chars[WHITE][QUEEN] = 0x2655;

    piece_chars[BLACK][B_PAWN] = 0x265f;
    piece_chars[BLACK][ROOK] = 0x265c;
    piece_chars[BLACK][KNIGHT] = 0x265e;
    piece_chars[BLACK][BISHOP] = 0x265d;
    piece_chars[BLACK][KING] = 0x265a;
    piece_chars[BLACK][QUEEN] = 0x265b;
}

void initialize_board(){
    for(int c = 0; c < 8; ++c){
        for(int r = 0; r < 8; ++r){
            board[c][r] = NULL;
        }
    }

    int col;
    int row;
    for(int i = 0; i < NTEAMS; ++i){
        for(int j = 0; j < TEAMSIZE; ++j){
            col = roster[i][j].pos.col;
            row = roster[i][j].pos.row;
            board[col][row] = &roster[i][j];
        }
    }
}
