#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <wchar.h>
#include "chess.h"

void print_coord_stack(coord_stack* cs){
    printf("coord stack %p:\n", cs);
    printf("iter: %d\n", cs->iter);
    printf("size: %d\n", cs->size);
    for(int i = 0; i < cs->iter; ++i){
        printf("  %d: <%d,%d>\n", i, cs->data[i]->col, cs->data[i]->row);
    }
}

void print_moves_list(){
    // iterate through move lists
    for(int i = 0; i < 7; ++i){
        printf("type %d:\n",i);
        int numPaths = moves[i].num_paths;
        // iterate through paths
        for(int j = 0; j < numPaths; ++j){
            printf("  p%d: ", j);
            // iterate through coords in path
            for(int k = 0; k < moves[i].paths[j].num_coords; ++k){
                coord* curr = &moves[i].paths[j].coords[k];
                printf("(%d,%d), ", curr->col, curr->row);
            }
            printf("\n");
        }
        printf("\n");
    }
}

void print_roster(){
    for(int i = 0; i < 2; ++i){
        printf("team: %d", i);
        for(int j = 0; j < 16; ++j){
            if(j%8 == 0)
                printf("\n  ");
            printf("%d:(%d,%d) ", roster[i][j].type, roster[i][j].pos.col, 
                roster[i][j].pos.row);
        }
        printf("\n");
    }
}

void print_board(){
    setlocale(LC_CTYPE, "");
    print_captured(BLACK);
    for(int row = 7; row >= 0; --row){
        for(int i = 0; i < 8; ++i)
            printf("-----"); 
        printf("-\n");
        for(char c = 'a'; c < 'i'; ++c){
            printf("|%c%d  ", c, row+1);
        }
        printf("|\n");
        for(int col = 0; col < 8; ++col){
            if(board[col][row] != NULL){
                COLOR c = board[col][row]->color;
                TYPE t = board[col][row]->type;
                wchar_t w = piece_chars[c][t];
                wprintf(L"|  %lc ", w);
            } else{
                printf("|    ");
            }
        }
        printf("|\n");
    }
    for(int i = 0; i < 8; ++i){
        printf("-----"); 
    }
    printf("-\n");
    print_captured(WHITE);
}

// color is of capturing team (not team being captured)
void print_captured(COLOR co){
    setlocale(LC_CTYPE, "");

    COLOR opponent = (-co) + 1;
    int count = 0;
    int firstLine = 0;
    printf(" _________________ \n");
    printf("|");

    for(int i = 0; i < TEAMSIZE; ++i){
        if(count == 8 && !firstLine){
            firstLine = 1;
            printf(" |\n");
            printf("|");
        }
        if(roster[opponent][i].captured == 0){
            continue;
        }
        ++count;
        TYPE t = roster[opponent][i].type;
        wchar_t w = piece_chars[opponent][t];
        wprintf(L" %lc", w);
    }
    // filling in the space
    while(count < 8){
        ++count;
        printf("  ");
    }
    if(count == 8 && !firstLine){
        printf(" |\n");
        printf("|");
    }
    while(count < 16){
        ++count;
        printf("  ");
    }
    if(count == 16){
        printf(" |\n");
    }
    printf(" ----------------- \n");
}
