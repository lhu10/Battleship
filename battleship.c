#include <stdio.h>
#include <stdlib.h> 
#include <string.h>

#include "battleship.h"
#define ROWS 10
#define COLUMNS 10
#define MAXSHIPS 5

char **init_board(){
    int i, j;  
    char **board = malloc(ROWS * sizeof(char *));
    for(i = 0; i < ROWS; i++){
        board[i] = malloc(COLUMNS * sizeof(char));
        for(j = 0; j < COLUMNS; j++){
            board[i][j] = '.';
        }
    }
    return board; 
}

void free_board(char **board){
    int i;
    for(i = 0; i < ROWS; i++){
        free(board[i]);
    }
    free(board);
}
//returns 0 if board generation failed
int generate_board(char *player, char **board, struct ship_node *ships){
    FILE *fp; 
    fp = fopen(player, "r");
    char buff[256];
    int i = 0;
    while(fgets(buff, sizeof(buff), fp)){
        *strchr(buff, '\n') = 0;   //gets rid of new line
        new_ship(ships, buff, i);
        int s = 0;
        int r = ships[i].row;
        int c = ships[i].column;
        char d = ships[i].direction;
        while(s != (ships[i].size)){
            int r_inc = s;
            int c_inc = 0;
            //if direction of ship is horizontal
            if(d == 'h'){
                r_inc = 0;
                c_inc = s;
            }
            if(check_board(r, r_inc, c, c_inc, board) == 0){
                fclose(fp);
                return 0; 
            }
            board[r+r_inc][c+c_inc] = '$';
            
            s++;
        }
        i++;
    }
    fclose(fp);
    return 1; 
}
//returns 1 if there are no errors
int check_board(int r, int r_inc, int c, int c_inc, char **board){
    //checks if column or row is out of bound
    if(( (c+c_inc)<0 || (c+c_inc)>9 ) || ( (r+r_inc)<0 || (r+r_inc)>9 )){
        printf("Error! Ship Out of Bound at %c%d\n", r+r_inc+65, c+c_inc+1);
        return 0;
    }
    if(board[r+r_inc][c+c_inc] != '.'){
        printf("Error! Ships overlap at %c%d\n", r+r_inc+65,c+c_inc+1);
        return 0;
    }
    return 1; 
}
//assumes syntax in file is correct
void new_ship(struct ship_node *ships, char *line, int i){
    (ships+i)->size = line[0] - '0';
    (ships+i)->lives = line[0] - '0';
    (ships+i)->direction = line[1];
    (ships+i)->row = line[2] - 'A';
    if(strlen(line)>2 && line[3] == '1' && line[4] == '0'){
        (ships+i)->column = 9;
    }
    else{
        (ships+i)->column = line[3] - '1';
    }
}
//returns 0 if invalid point
int launch_missile(char *coord, char **board, char **sboard, struct ship_node *ships){
    *strchr(coord, '\n') = 0;   //gets rid of new line
    int row = coord[0]-'A';
    int column = coord[1]-'1';
    //if input column greater than 9
    if(strlen(coord) > 2){
        //column is 10 so index in array is 9
        if(coord[1] == '1' && coord[2] == '0')
            column = 9;
        else{
            return 0; 
        }
    }
    //checks if point is out of bounds
    if(row < 0 || row > ROWS-1 || column < 0 || column > COLUMNS-1){
        return 0; 
    }
    if(board[row][column] == '$'){
        int boolean = 0; //false 
        int i = 0;
        while(boolean == 0){ 
            //if coord matches head of ship coords
            if(row == ships[i].row && column == ships[i].column){
                boolean = 1; 
            } //separate into function
            else if((row == ships[i].row &&
                    abs(ships[i].column - column) < ships[i].size &&
                    ships[i].direction == 'h' &&
                    ships[i].column - column < 0) ||
                    (column == ships[i].column &&
                    abs(ships[i].row - row) < ships[i].size &&
                    ships[i].direction == 'v' &&
                    ships[i].row - row < 0)){
                boolean = 1; 
            }
            else
                i++;
        }
        ships[i].lives--; 
        check_downed_ship(row, column, i, board, sboard, ships);
    }
    else if(board[row][column] == '.'){
        board[row][column] = 'O'; //missed
        sboard[row][column] = 'O';
    }
    return 1; 
}

void check_downed_ship(int row, int column, int i, char **board, char **sboard, struct ship_node *ships){
    if(ships[i].lives == 0){
        int s = 0;
        int r = ships[i].row;
        int c = ships[i].column;
        char d = ships[i].direction;
        while(s != (ships[i].size)){
            int r_inc = s;
            int c_inc = 0;
            //if direction of ship is horizontal
            if(d == 'h'){
                r_inc = 0;
                c_inc = s;
            }
            board[r+r_inc][c+c_inc] = '#';
            sboard[r+r_inc][c+c_inc] = '#';
            s++;
        }
    }
    else{
        board[row][column] = '*';
        sboard[row][column] = '*';
    }
}
//returns 1 if all ships cleared and player wins
int check_win(char **board, struct ship_node *ships){
    int i; 
    int count = 0; 
    for(i = 0; i < MAXSHIPS; i++){
        if(ships[i].lives == 0){
            count++; 
        }
    }
    if(count == MAXSHIPS){
        return 1; 
    }
    return 0; 
}

void display_board(char **board){
    int i, j;
    printf("   ");
    for(i = 0; i < ROWS; i++){
        printf("%d  ", i+1);
    }
    printf("\n");
    for(i = 0; i < ROWS; i++){
        printf("%c  ", i+65);
        for(j = 0; j < COLUMNS; j++){
            printf("%c  ", board[i][j]);
        }
        printf("\n");
    }
}

char *two_to_one(char **board){
    char *arr = (char *)malloc(101 * sizeof(char));
    int i, j, k = 0; 
    for(i = 0; i < ROWS; i++){
        for(j = 0; j < COLUMNS; j++){
            arr[k] = board[i][j];
            k++; 
        }
    }
    return arr; 
}

void one_to_two(char *arr, char **board){
    int i, j, k = 0; 
    for(i = 0; i < ROWS; i++){
        for(j = 0; j < COLUMNS; j++){
            board[i][j] = arr[k];
            k++; 
        }
    }
}

void end_prog(char *arr, char **board, char **board1, char **board2){
    free(arr);
    free_board(board);
    free_board(board1);
    free_board(board2);
    exit(0);
}
