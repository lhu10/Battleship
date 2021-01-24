#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <signal.h>

#include "battleship.h"
#define MAXSHIPS 5

volatile sig_atomic_t flag = 0;
static void sighandler(int signo){
    if(signo == SIGINT){
        flag = 1; 
    }
}

int main(){
    signal(SIGINT, sighandler);
    char **board = init_board();
    display_board(board);
    struct ship_node ships[MAXSHIPS];
    generate_board("player1", board, ships);
    display_board(board);
    
    char buff[256]; 
    while(flag == 0){
        printf("Input coord\n");
        fgets(buff, sizeof(buff), stdin); 

        int check = launch_missile(buff, board, ships);
        while(check == 0){
            printf("Input another coord\n");
            fgets(buff, sizeof(buff), stdin); 
            check = launch_missile(buff, board, ships);
        }

        display_board(board);
        
        int win = check_win(board, ships);
        if(win == 1){
           end_prog(board);
        }
    }

    printf("You surrendered\n");
    end_prog(board);

    return 0; 
}
