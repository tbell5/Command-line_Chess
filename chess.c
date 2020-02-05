/***********************
 |  Tucker Bell, 2020  |
 ***********************/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <locale.h>
#include <wchar.h>
#include "chess.h"

void run_game();
int custom_setup(char*); // return 1 if white start, 2 if black, 0 fail
int main(){
    initialize_game();
    run_game();
}

void run_game(){
    char sbuf[100];
    int csRet = 0;
    /* 
    printf("custom starting setup? (y/n): ");
    fgets(sbuf, 100, stdin);
    if(sbuf[0] == 'y'){
        printf("enter initializing file: ");
        fgets(sbuf, 100, stdin);
        sbuf[strlen(sbuf) - 1] = '\0';  // trim newline from fgets
        if((csRet = custom_setup(sbuf)) == 0){
            printf("setup failed, aborting\n");
            exit(0);
        }
    }
    */

    COLOR cur_team = (csRet == 2 ? BLACK : WHITE);
    COLOR prev_team = -cur_team + 1;
    int moveCount = 0;
    while(1){
        if(prev_team != cur_team){
            print_board();
            // evaluate check, checkmate, stalemate
            if(is_check(cur_team)){
                if(is_mate(cur_team)){
                    printf("Checkmate %s.\n", color_strings[cur_team]);
                    exit(0);
                }
                printf("Check,\n");
            } else if(is_stale(cur_team)){
                printf("Stalemate.\n");
                exit(0);
            }
        }
        prev_team = cur_team;

        // select piece
        int bufSize = 10;
        char buf[10];
        printf("%s's move, select piece: ", color_strings[cur_team]);
        fgets(buf, 10, stdin);
        // special commands 
        if(strlen(buf) == 2){
            int ic, im, ism;
            switch(buf[0]){
                case 'k':
                    // ic = is_check(cur_team);
                    // im = is_mate(cur_team);
                    ism = is_stale(cur_team);
                    // printf("is check: %s\n", ic ? "true" : "false");
                    // printf("is mate: %s\n", im ? "true" : "false");
                    printf("is stale mate: %s\n", ism ? "true" : "false");
                    continue;
                /* 
                case 'u':
                    if(moveCount > 0){
                        undoMove();
                        --moveCount;
                        cur_team = -cur_team + 1;
                        continue;
                    } else{
                        continue;
                    }
                */
                case 'q':
                    exit(0);
                default:
                    break;
            }
        }
        if(strlen(buf) < 2 || !islower(buf[0]) || !isdigit(buf[1])){
            printf("Bad input, enter valid square using lower-case.\n");
            continue;
        }
        // set piece
        int col = 0, row = 0;
        coord curC;
        piece* curP;
        col = (int)(buf[0]-'a');
        row = (int)(buf[1]-'1');
        setCoord(col, row, &curC);
        if(!isInbounds(&curC)){
            printf("Out of bounds: <%d,%d>.\n", curC.col, curC.row);
            continue;
        } else if (isEmptySpace(&curC)){
            printf("Empty space: <%d,%d>.\n", curC.col, curC.row);
            continue;
        } else if((curP = getSquare(&curC)) != NULL && 
            curP->color != cur_team){
            printf("Wrong team: <%d,%d>: %s.\n", curC.col, curC.row, 
                color_strings[curP->color]);
            continue;
        } else{
            printf("Selected %s %s at <%d,%d>.\n", color_strings[curP->color],
                piece_strings[curP->type], curC.col, curC.row);
        }

        // select move
        printf("Select move: ");
        memset(buf, 0, bufSize);
        fgets(buf, 10, stdin);
        if(strlen(buf) < 2 || !islower(buf[0]) || !isdigit(buf[1])){
            printf("Bad input; enter valid square using lower-case.\n");
        }
        col = (int)(buf[0]-'a');
        row = (int)(buf[1]-'1');
        setCoord(col, row, &curC);

        // try move
        if(move(curP, &curC) == 0){
            printf("Invalid move.\n");
            continue;
        }
        if(is_check(curP->color)){
            printf("Cannot move into check.\n");
            undoMove();
            continue;
        }
        if((curP->type == W_PAWN && curP->pos.row == 7) || 
            (curP->type == B_PAWN && curP->pos.row == 0)){
            promote(curP);  // pawn promotion
        }
        ++moveCount;
        // give up turn
        cur_team = -cur_team + 1;
    }
}

/* board functions */

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

piece* getSquare(coord* c){
    if(!isInbounds(c)){
        return NULL;
    }
    return board[c->col][c->row];
}

int setSquareAndPiece(coord* c, piece* p){
    if(isInbounds(c)){
        board[c->col][c->row] = p;
        if(p != NULL) {
            updateCoord(&p->pos, c);
        }
        return 1;
    }
    return 0;
}

// can color c attack square at coord c
coord_stack* get_square_reachers(coord* c, COLOR co){
    if(!isInbounds(c)){
        return NULL;
    }
    coord_stack* newStack = coord_stack_init();
    for(int i = 0; i < TEAMSIZE; ++i){
        if(find_move(&roster[co][i], c) != NONE){
            coord_stack_push(newStack, &roster[co][i].pos);
        }
    }
    if(newStack->iter == 0){
        coord_stack_free(newStack);
        newStack = NULL;
    }
    return newStack;
}

// is team co in check
int is_check(COLOR co){
    int ret = 0;
    coord_stack* curCs = get_checkers(co);
    if(curCs != NULL){
        ret = 1;
    }
    coord_stack_free(curCs);
    return ret;
}

// return coords of pieces which can attack the co king, NULL if none can
coord_stack* get_checkers(COLOR co){
    piece* king = NULL;
    for(int i = 0; i < TEAMSIZE; ++i){
        if(roster[co][i].type == KING){
            king = &roster[co][i];
            break;
        }
    }
    if(king == NULL){
        return NULL;
    }

    COLOR coOp = -co + 1;
    return get_square_reachers(&king->pos, coOp);
}

int is_mate(COLOR co){
    piece* king = NULL;
    for(int i = 0; i < TEAMSIZE; ++i){
        if(roster[co][i].type == KING){
            king = &roster[co][i];
            break;
        }
    }
    if(king == NULL){
        return 0;
    }
    COLOR coOp = -co + 1;

    // if king can move out of check, return 0;
    // works for both empty spaces and occupied spaces
    coord kTemp;
    for(int i = -1; i <= 1; ++i){
        for(int j = -1; j <= 1; ++j){
            if(i == 0 && j == 0) {
                continue;
            }
            updateCoord(&kTemp, &king->pos);
            kTemp.col += i;
            kTemp.row += j;
            if(move(king, &kTemp)){
                if(!is_check(co)){
                    undoMove();
                    return 0;
                }
                undoMove();
            }
        }
    }

    // if there are more than two attackers, is mate, since the
    // king cannot move out of it and all cannot be blocked/captured
    coord_stack* attackers = get_checkers(co);
    if(attackers == NULL){
        return 0;
    } else if(attackers->iter > 1){
        coord_stack_free(attackers);
        return 1;
    }
    coord_stack_free(attackers);
    piece* attacker = getSquare(attackers->data[0]);

    // check if can be blocked:
    coord* curSq;
    coord_stack* blockers;
    piece* curBl;
    coord_stack* interms = find_intermediate_squares(attacker, &king->pos);
    // ...for each square between king and attacker:
    for(int i = 0; interms != NULL && i < interms->iter; ++i){
        curSq = interms->data[i];
        blockers = get_square_reachers(curSq, co);
        if(blockers == NULL){
            coord_stack_free(blockers);
            continue;
        }
        // ...for each piece which can block cur square:
        for(int j = 0; j < blockers->iter; ++j){
            curBl = getSquare(blockers->data[j]);
            if(move(curBl, curSq)){
                if(!is_check(co)){
                    undoMove();
                    coord_stack_free(blockers);
                    coord_stack_free(interms);
                    return 0;
                }
                undoMove();
            }
        }
        coord_stack_free(blockers);
    }
    coord_stack_free(interms);

    // check if can be counter-attacked:
    coord_stack* coAttackers = get_square_reachers(&attacker->pos, co);
    if(coAttackers == NULL){
        return 1;
    }
    // ...for each piece which can reach attacker:
    for(int i = 0; i < coAttackers->iter; ++i){
        piece* coAttacker = getSquare(coAttackers->data[i]);
        if(move(coAttacker, &attacker->pos)){
            if(!is_check(co)){
                undoMove();
                coord_stack_free(coAttackers);
                return 0;
            }
            undoMove();
        }
    }
    coord_stack_free(coAttackers);
    return 1;
}

// file format:
// <piece type>,<square>
// ...
// #
// <piece type>,<square>
// ...
// #
// <b/[leave blank]>
int custom_setup(char* file){
    FILE* fp = fopen(file, "r");
    if(fp == NULL){
        printf("error: custom_setup: bad file: <%s>\n", file);
        return 0;
    }

    // clear board
    for(int i = 0; i < 8; ++i){
        for(int j = 0; j < 8; ++j){
            board[i][j] = NULL;
        }
    }
    // set all pieces to -1,-1
    for(int i = 0; i < 2; ++i){
        for(int j = 0; j < 16; ++j){
            roster[i][j].pos.col = -1;
            roster[i][j].pos.row = -1;
            roster[i][j].captured = 1;
        }
    }

    char buf[50];
    COLOR co = WHITE;
    TYPE curType;
    int curCol, curRow, jmpInt;
    coord curCoord;

    int lineCount = 0;
    while(fgets(buf, 50, fp) > 0){
        ++lineCount;
        if(buf[0] == '#'){
            if(co == BLACK){
                // after initializing pieces, can set curTeam to black
                if(fgets(buf, strlen(buf), fp) > 0){
                    if(buf[0] == 'b'){
                        fclose(fp);
                        return 2;
                    }
                }
                fclose(fp);
                return 1;
            }
            co = BLACK;
            continue;
        }
        // read in piec type, position, and jump state
        char* piece = strtok(buf, ",");
        char* pos = strtok(NULL, ",");
        char* jmp = strtok(NULL, ",");
        if(piece == NULL || pos == NULL){
            printf("error: custom_setup: bad line: %d\n", lineCount);
            fclose(fp);
            return 0;
        }
        // match type string to enum
        if(strcmp(piece, "ROOK") == 0){
            curType = ROOK;
        } else if(strcmp(piece, "KNIGHT") == 0){
            curType = KNIGHT;
        } else if(strcmp(piece, "BISHOP") == 0){
            curType = BISHOP;
        } else if(strcmp(piece, "QUEEN") == 0){
            curType = QUEEN;
        } else if(strcmp(piece, "KING") == 0){
            curType = KING;
        } else if(strcmp(piece, "W_PAWN") == 0){
            curType = W_PAWN;
        } else if(strcmp(piece, "B_PAWN") == 0){
            curType = B_PAWN;
        } else{
            printf("error: custom_setup: bad piece type: %s\n", piece);
            fclose(fp);
            return 0;
        }
        // ensure proper position number and generate coord 
        if(strlen(pos) < 2 || (pos[0] < 'A' || pos[0] > 'H') || (pos[1] < '1' || pos[1] > '8')){
            printf("error: custom_setup: bad position: %s\n", pos);
            fclose(fp);
            return 0;
        }
        curCol = pos[0] - 'A';
        curRow = pos[1] - '1';
        setCoord(curCol, curRow, &curCoord);
        // get jump state (for pawns)
        jmpInt = 0;
        if(jmp != NULL && jmp[0] == 1){
            jmpInt = 1;
        }
        // find a matching piece type in roster and set it
        int found = 0;
        for(int i = 0; i < 16; ++i){
            if(roster[co][i].type == curType && roster[co][i].pos.col == -1){
                if(jmpInt){
                    roster[co][i].jump = 1;
                }
                setSquareAndPiece(&curCoord, &roster[co][i]);
                roster[co][i].captured = 0;
                found = 1;
                break;
            }
        }
        if(!found){
            printf("error: custom_setup: no matching piece found for line %d\n", lineCount);
            fclose(fp);
            return 0;
        }
    }

    return 1;
}

int is_stale(COLOR co){
    // if a single piece can move, return false
    for(int i = 0; i < TEAMSIZE; ++i){
        if(can_move(&roster[co][i])){
            return 0;
        }
    }
    return 1;
}
