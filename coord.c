#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <locale.h>
#include <wchar.h>
#include "chess.h"

/* coord functions */

coord* newCoord(int col, int row){
    coord* new = malloc(sizeof(coord));
    new->col = col;
    new->row = row;
    return new;
}

coord* copyCoord(coord* c){
    if(c == NULL)
        return NULL;
    return newCoord(c->col, c->row);
}

void setCoord(int col, int row, coord* cur){
    cur->col = col;
    cur->row = row;
}

void addCoords(coord* sum, coord* add){
    sum->col += add->col;
    sum->row += add->row;
}

void updateCoord(coord *old, coord *new){
    if(old == NULL || new == NULL)
        return;
    old->col = new->col;
    old->row = new->row;
}

int compareCoords(coord* a, coord* b){
    if(a->col == b->col && a->row == b->row){
        return 1;
    }
    return 0;
}

/* coord stack functions */

coord_stack* coord_stack_init(){
    coord_stack* new = malloc(sizeof(coord_stack));
    new->iter = 0;
    new->size = CS_ISIZE;
    new->data = malloc(sizeof(coord*)*new->size);
    return new;
}

void coord_stack_push(coord_stack* cs, coord* c){
    if(cs->iter >= cs->size){
        cs->size *= 2;
        cs->data = realloc(cs->data, sizeof(coord*)*cs->size);
    }
    cs->data[cs->iter] = copyCoord(c);
    cs->iter += 1;
}

// a deep search, ie compares row/col values
// TODO change to return boolean
int coord_stack_search(coord_stack* cs, coord* c){
    for(int i = 0; i < cs->iter; ++i){
        if(cs->data[i]->col == c->col && cs->data[i]->row == c->row){
            return 1;
        }
    }
    return 0;
}

void coord_stack_free(coord_stack* cs){
    if(cs == NULL)
        return;
    for(int i = 0; i < cs->iter; ++i)
        free(cs->data[i]);
    free(cs);
    return;
}