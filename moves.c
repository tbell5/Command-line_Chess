#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <locale.h>
#include <wchar.h>
#include "chess.h"

/* move functions */

int move(piece* p, coord* c){
    // log things
    coord start = p->pos;
    coord end = *c;
    piece* secondary = NULL; // for log
    // safety check that boord is properly synced w/piece
    if(p != getSquare(&p->pos)){
        printf("error: move: piece/board mismatch\n");
        return 0;
    }
    // check if valid move
    PATH_TYPE pt = find_move(p, c);
    coord rc;   // for castling (below)
    switch(pt){
        case NONE:
            return 0;
        case PAWN_CAPTURE:
            if(enPassantFlag){
                // set secondary
                coord epc;
                updateCoord(&epc, c);
                if (p->color == WHITE) {
                    epc.row -= 1;
                } else {
                    epc.row += 1;
                }
                secondary = getSquare(&epc);
                // capture
                enPassant(p, c);
            }
            break;
        case PAWN_JUMP:
            p->jump = 1;
            break;
        case PAWN_STEP:
            break;
        case Q_CASTLE:
            q_castle(p, c);
            updateCoord(&rc, c);
            rc.col += 1;
            secondary = getSquare(&rc);
            break;
        case K_CASTLE:
            k_castle(p, c);
            updateCoord(&rc, c);
            rc.col -= 1;
            secondary = getSquare(&rc);
            break;
        default:
            break;
    }
    // execute move
    if(!isEmptySpace(c)){
        secondary = getSquare(c);
        secondary->captured = 1;
    }
    p->nMoves += 1;
    setSquareAndPiece(c, p);
    setSquareAndPiece(&start, NULL);

    pushLog(p, secondary, start, end, pt, enPassantFlag);
    enPassantFlag = 0;
    clearJumps(-p->color + 1);  // clear opponent jumps after we move

    return 1;
}

// is the move to either an empty space or opposite team's space
int is_valid_move(piece* p, coord* c){
    if(c == NULL){
        return 0;
    }
    if(!isInbounds(c)){
        // printf("move out of bounds\n");
        return 0;
    }
    piece* curr;
    if((curr = getSquare(c)) != NULL && curr->color == p->color){
        return 0;
    }
    return 1;
}

// is the move to the opposite team's space
int is_attack(piece* p, coord* c){
    if(!is_valid_move(p, c)){
        return 0;
    }
    if(getSquare(c) == NULL){
        return 0;
    }
    return 1;
}

PATH_TYPE find_move(piece* p, coord* c){
    if(p == NULL || c == NULL || !isInbounds(c)|| p->captured == 1){
        return NONE;
    }
    moves_list *ml = &moves[p->type];
    coord temp;

    for(int pn = 0; pn < ml->num_paths; ++pn){
        int numValidMoves = traversePath(&ml->paths[pn], p);
        for(int i = 0; i < numValidMoves; ++i){
            // calculate position for move, see if it matches c
            updateCoord(&temp, &ml->paths[pn].coords[i]);
            addCoords(&temp, &p->pos);
            if(compareCoords(&temp, c)){
                switch(ml->paths[pn].path_type){
                    case PAWN_CAPTURE:
                        if(!is_pawn_capture(p, c)){
                            return NONE;
                        }
                        break;
                    case PAWN_JUMP:
                        if(!is_pawn_jump(p, c)){
                            return NONE;
                        }
                        break;
                    case PAWN_STEP:
                        if(!is_pawn_step(p, c)){
                            return NONE;
                        }
                        break;
                    case Q_CASTLE:
                        if(!is_q_castle(p, c)){
                            return NONE;
                        }
                        break;
                    case K_CASTLE:
                        if(!is_k_castle(p, c)){
                            return NONE;
                        }
                        break;
                    default:
                        break;
                }
                return ml->paths[pn].path_type;
            }
        }
    }
    return NONE;
}

PATH_TYPE find_move_no_castle(piece* p, coord* c){
    if(p == NULL || c == NULL || !isInbounds(c)|| p->captured == 1){
        return NONE;
    }
    moves_list *ml = &moves[p->type];
    coord temp;

    for(int pn = 0; pn < ml->num_paths; ++pn){
        int numValidMoves = traversePath(&ml->paths[pn], p);
        for(int i = 0; i < numValidMoves; ++i){
            // calculate position for move, see if it matches c
            updateCoord(&temp, &ml->paths[pn].coords[i]);
            addCoords(&temp, &p->pos);
            if(compareCoords(&temp, c)){
                switch(ml->paths[pn].path_type){
                    case PAWN_CAPTURE:
                        if(!is_pawn_capture(p, c)){
                            return NONE;
                        }
                        break;
                    case PAWN_JUMP:
                        if(!is_pawn_jump(p, c)){
                            return NONE;
                        }
                        break;
                    case PAWN_STEP:
                        if(!is_pawn_step(p, c)){
                            return NONE;
                        }
                        break;
                    case Q_CASTLE:
                        return NONE;
                    case K_CASTLE:
                        return NONE;
                    default:
                        break;
                }
                return ml->paths[pn].path_type;
            }
        }
    }
    return NONE;
}

coord_stack* get_moves(piece* p){
    coord_stack* newCs = coord_stack_init();
    moves_list *ml = &moves[p->type];
    coord temp;

    for(int pn = 0; pn < ml->num_paths; ++pn){
        int numValidMoves = traversePath(&ml->paths[pn], p);
        for(int i = 0; i < numValidMoves; ++i){
            updateCoord(&temp, &ml->paths[pn].coords[i]);
            addCoords(&temp, &p->pos);
            coord_stack_push(newCs, &temp);
        }
    }
    return newCs;
}

int traversePath(path* pa, piece* p){
    if(pa->path_type != REGULAR){
        // assume all coords in irregular path are valid, check elsewhere
        return pa->num_coords;
    }
    int moveCount = 0;
    coord temp;
    for(int i = 0; i < pa->num_coords; ++i){
        updateCoord(&temp, &pa->coords[i]);
        addCoords(&temp, &p->pos);
        if(is_valid_move(p, &temp)){
            ++moveCount;
            if(is_attack(p, &temp)){
                return moveCount;
            }
        } else{
            return moveCount;
        }
    }
    return moveCount;
}

int is_pawn_capture(piece* p, coord* c){
    // normal captures
    piece* target = getSquare(c);
    if(p->color == WHITE && target != NULL && target->color == BLACK){
        return 1;
    } else if(p->color == BLACK && target != NULL && target->color == WHITE){
        return 1;
    } else if(target != NULL){
        return 0;
    }
    // en passant, target is NULL
    coord temp;
    if(p->color == BLACK && p->pos.row == 3){
        updateCoord(&temp, c);
        temp.row += 1;
        target = getSquare(&temp);
        if(target != NULL && target->color == WHITE && target->type == W_PAWN
            && target->nMoves == 1 && target->jump == 1){
            enPassantFlag = 1;
            return 1;
        }
    } else if(p->color == WHITE && p->pos.row == 4){
        updateCoord(&temp, c);
        temp.row -= 1;
        target = getSquare(&temp);
        if(target != NULL && target->color == BLACK && target->type == B_PAWN
            && target->nMoves == 1 && target->jump == 1){
            enPassantFlag = 1;
            return 1;
        }
    }
    return 0;
}

int is_pawn_jump(piece* p, coord* c){
    if(getSquare(c) != NULL || p->nMoves > 0){
        return 0;
    }
    coord temp;
    updateCoord(&temp, c);
    if(p->color == WHITE){
        temp.row -= 1;
    } else{
        temp.row += 1;
    }
    if(getSquare(&temp) == NULL){
        return 1;
    }
    return 0;
}

int is_pawn_step(piece* p, coord* c){
    if(getSquare(c) != NULL){
        return 0;
    }
    return 1;
}

int is_k_castle(piece* p, coord* c){
    // check if king is in check, using only get_move_no_castle
    COLOR coOp = -p->color + 1;
    for(int i = 0; i < TEAMSIZE; ++i){
        if(find_move_no_castle(&roster[coOp][i], &p->pos) != NONE){
            return 0;
        }
    }

    // if on first or last rank
    if(p->pos.row != 0 && p->pos.row != 7){
        return 0;
    }
    // if king has not moved yet
    if(p->nMoves > 0){
        return 0;
    }
    // if square at coord is null
    // if square to left of coord is null
    coord midC;
    updateCoord(&midC, c);
    midC.col -= 1;
    if(getSquare(&midC) != NULL || getSquare(c) != NULL){
        return 0;
    }
    // if rook is to right of coord
    // if rook is right color, hasn't moved
    coord rookC;
    updateCoord(&rookC, c);
    rookC.col += 1;
    piece* rook = getSquare(&rookC);
    if(rook == NULL || rook->type != ROOK || rook->nMoves > 0){
        return 0;
    }

    return 1;
}

int is_q_castle(piece* p, coord* c){
    coord midCr;
    coord midCl;
    updateCoord(&midCr, c);
    updateCoord(&midCl, c);
    midCr.col += 1;
    midCl.col -= 1;

    // check if team in check, using only get_move_no_castle
    // check if left-of square in check, using only get_move_no_castle
    COLOR coOp = -p->color + 1;
    for(int i = 0; i < TEAMSIZE; ++i){
        if(find_move_no_castle(&roster[coOp][i], &p->pos) != NONE
           || find_move_no_castle(&roster[coOp][i], &midCl) != NONE){
            return 0;
        }
    }

    if(p->pos.row != 0 && p->pos.row != 7){
        return 0;
    }
    if(p->nMoves > 0){
        return 0;
    }
    if(getSquare(&midCl) != NULL || getSquare(&midCr) != NULL){
        return 0;
    }
    coord rookC;
    updateCoord(&rookC, c);
    rookC.col -= 2;
    piece* rook = getSquare(&rookC);
    if(rook == NULL || rook->type != ROOK || rook->nMoves > 0){
        return 0;
    }
    return 1;
}

// captures opponent pawn, does not move attacking pawn
void enPassant(piece* p, coord* c){
    piece* targetPawn;
    coord tpc;
    updateCoord(&tpc, c);
    if(p->color == WHITE){
        tpc.row -= 1;
    } else {
        tpc.row += 1;
    }
    getSquare(&tpc)->captured = 1;
    setSquareAndPiece(&tpc, NULL);
}

void q_castle(piece* p, coord* c){
    piece* rook;
    coord rc;
    updateCoord(&rc, c);
    rc.col -= 2;
    rook = getSquare(&rc);

    coord mv;
    updateCoord(&mv, c);
    mv.col += 1;
    setSquareAndPiece(&mv, rook);
    setSquareAndPiece(&rc, NULL);
    rook->nMoves += 1;
}

void k_castle(piece* p, coord* c){
    piece* rook;
    coord rc;
    updateCoord(&rc, c);
    rc.col += 1;
    rook = getSquare(&rc);

    coord mv;
    updateCoord(&mv, c);
    mv.col -= 1;
    setSquareAndPiece(&mv, rook);
    setSquareAndPiece(&rc, NULL);
    rook->nMoves += 1;
}

void promote(piece* p){
    char buf[10];
    printf("to promote piece, type \"q\", \"k\", \"r\", or \"b\": ");
    while(1){
        fgets(buf, 10, stdin);
        switch(buf[0]){
            case 'q':
                p->type = QUEEN;
                return;
            case 'k':
                p-> type = KNIGHT;
                return;
            case 'r':
                p->type = ROOK;
                return;
            case 'b':
                p->type = BISHOP;
                return;
            default:
                printf("bad input\n");
        }
    }
}

void clearJumps(COLOR co){
    for(int i = 0; i < TEAMSIZE; ++i){
        roster[co][i].jump = 0;
    }
}

void pushLog(piece* primary, piece* secondary, coord start, coord end, PATH_TYPE path, int ep) {
    if(move_log.iter >= move_log.size){
        move_log.size *= 2;
        move_log.data = realloc(move_log.data, sizeof(log_node)*move_log.size);
    }
    int curit = move_log.iter;
    move_log.data[curit].primary = primary;
    move_log.data[curit].secondary = secondary;
    move_log.data[curit].primary_type = primary->type;
    move_log.data[curit].start = start;
    move_log.data[curit].end = end;
    move_log.data[curit].pathType = path;
    move_log.data[curit].enPassant = ep;
    ++move_log.iter;
}

log_node popLog(){
    log_node new;
    memset(&new, 0, sizeof(log_node));
    if(move_log.iter <= 0){
        return new;
    }
    new = move_log.data[move_log.iter - 1];
    --move_log.iter;
    return new;
}

void undoMove(){
    log_node curNode = popLog();
    if(curNode.primary == NULL){
        return;
    }
    piece* primary = curNode.primary;
    piece* secondary = curNode.secondary;
    TYPE primaryType = curNode.primary_type;
    coord start = curNode.start;
    coord end = curNode.end;
    PATH_TYPE pt = curNode.pathType;
    int isEp = curNode.enPassant;

    setSquareAndPiece(&start, primary);
    --primary->nMoves;
    primary->type = primaryType;
    if(pt == REGULAR || pt == PAWN_STEP || pt == PAWN_JUMP || (pt == PAWN_CAPTURE && !isEp)){
        setSquareAndPiece(&end, secondary);
        if(secondary != NULL){
            secondary->captured = 0;
        }
        primary->jump = 0;  // if this move was a pawn jump
    } else if(pt == PAWN_CAPTURE && isEp){
        setSquareAndPiece(&end, NULL);
        coord temp;
        updateCoord(&temp, &end);
        if(primary->color == WHITE){
            temp.row -= 1;
        } else{
            temp.row += 1;
        }
        setSquareAndPiece(&temp, secondary);
        secondary->captured = 0;
    } else if (pt == Q_CASTLE || pt == K_CASTLE) {
        setSquareAndPiece(&end, NULL);
        setSquareAndPiece(&secondary->pos, NULL);
        coord temp;
        updateCoord(&temp, &end);
        if(pt == Q_CASTLE){
            temp.col -= 2;
        } else {
            temp.col += 1;
        }
        setSquareAndPiece(&temp, secondary);
        --secondary->nMoves;
    }

    if(move_log.iter > 0 && move_log.data[move_log.iter - 1].pathType == PAWN_JUMP){
        move_log.data[move_log.iter - 1].primary->jump = 1;
    }
}

coord_stack* find_intermediate_squares(piece* p, coord* c){
    if(p == NULL || c == NULL || p->captured == 1){
        return NULL;
    }
    coord aCo = p->pos;
    moves_list *ml = &moves[p->type];

    coord temp;
    coord_stack* newStack = coord_stack_init();
    for(int pn = 0; pn < ml->num_paths; ++pn){
        int numValidMoves = traversePath(&ml->paths[pn], p);
        for(int i = 0; i < numValidMoves; ++i){
            // calculate position for move, see if it matches c
            updateCoord(&temp, &ml->paths[pn].coords[i]);
            addCoords(&temp, &p->pos);
            if(compareCoords(&temp, c)){
                // path should already have been verified valid, 
                // but check for safety
                switch(ml->paths[pn].path_type){
                    case PAWN_CAPTURE:
                        if(!is_pawn_capture(p, c)){
                            goto free;
                        }
                        break;
                    case PAWN_JUMP:
                        if(!is_pawn_jump(p, c)){
                            goto free;
                        }
                        break;
                    case PAWN_STEP:
                        if(!is_pawn_step(p, c)){
                            goto free;
                        }
                        break;
                    case Q_CASTLE:
                        if(!is_q_castle(p, c)){
                            goto free;
                        }
                        break;
                    case K_CASTLE:
                        if(!is_k_castle(p, c)){
                            goto free;
                        }
                        break;
                    default:
                        break;
                }
                // put the earlier, "intermediate" squares in the coord_stack
                --i;
                while(i >= 0){
                    updateCoord(&temp, &ml->paths[pn].coords[i]);
                    addCoords(&temp, &p->pos);
                    coord_stack_push(newStack, &temp);
                    --i;
                }
                goto free;  // break both for loops
            }
        }
    }
free:
    if(newStack->iter == 0){
        coord_stack_free(newStack);
        newStack = NULL;
    }
    return newStack;
}

int can_move(piece* p){
    if(p == NULL || p->captured == 1){
        return 0;
    }

    moves_list *ml = &moves[p->type];
    coord temp;
    int validMove;
    for(int pn = 0; pn < ml->num_paths; ++pn){
        int numValidMoves = traversePath(&ml->paths[pn], p);
        for(int i = 0; i < numValidMoves; ++i){
            // calculate position for move
            updateCoord(&temp, &ml->paths[pn].coords[i]);
            addCoords(&temp, &p->pos);
            validMove = 0;
            switch(ml->paths[pn].path_type){
                case PAWN_CAPTURE:
                    if(is_pawn_capture(p, &temp)){
                        validMove = 1;
                    }
                    break;
                case PAWN_JUMP:
                    if(is_pawn_jump(p, &temp)){
                        validMove = 1;
                    }
                    break;
                case PAWN_STEP:
                    if(is_pawn_step(p, &temp)){
                        validMove = 1;
                    }
                    break;
                case Q_CASTLE:
                    if(is_q_castle(p, &temp)){
                        validMove = 1;
                    }
                    break;
                case K_CASTLE:
                    if(is_k_castle(p, &temp)){
                        validMove = 1;
                    }
                    break;
                default:    // REGULAR
                    validMove = 1;
            }
            if(!validMove){
                continue;
            }
            if(move(p, &temp)){
                if(!is_check(p->color)){
                    undoMove();
                    return 1;
                }
                undoMove();
            }
        }
    }
    return 0;
}