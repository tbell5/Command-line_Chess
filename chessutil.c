#include "chess.h"
#include <stdio.h>

int isInbounds(coord* c){
    if(c->col > 7 || c->col < 0 || c->row > 7 || c->col < 0)
        return 0;
    return 1;
}

int isEmptySpace(coord* c){
    if(!isInbounds(c))
        return 0;
    if(board[c->col][c->row] != NULL)
        return 0;
    return 1;
}