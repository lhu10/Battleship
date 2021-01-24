#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>
#include "battleship.h"
#define MAXSHIPS 5

volatile sig_atomic_t flag = 0;
static void sighandler(int signo){
    if(signo == SIGINT){
        unlink("input");
        unlink("output");
        unlink("public");
        flag = 1; 
    }
}

void check_error(int check){
    if(check == -1){
        printf("Error %d: %s\n", errno, strerror(errno));
    }
}

void handshake(){
    char pipe[256], message[100];
    mkfifo("public", 0666);
    printf("Server awaiting new Client\n");

    int fd_write, fd_read, check;
    fd_read = open("public", O_RDONLY);
    check_error(fd_read);
    //server receives client's message
    check = read(fd_read, pipe, sizeof(pipe));
    check_error(check);
    printf("Server receiving Client's message\n");
    printf("Server removing WKP\n");
    unlink("public");

    fd_write = open(pipe, O_WRONLY);
    check_error(fd_write);
    //server sends response to client
    char response[100] = "initial acknowledgement message";
    check = write(fd_write, response, sizeof(response));
    check_error(check);

    //server receives final client's message
    check = read(fd_read, message, sizeof(message));
    check_error(check);
    printf("%s\n", message);

    close(fd_write);
    close(fd_read);
}

void operation(){
    int fd_in, fd_out;
    fd_in = open("input", O_RDONLY);
    fd_out = open("output", O_WRONLY);

    char **p1_board = init_board(); 
    struct ship_node p1_ships[MAXSHIPS];
    generate_board("player1", p1_board, p1_ships);
    printf("This is player 1 board\n");
    display_board(p1_board);
    
    //copy of p1_board without visible ships to be sent to client and displayed in client
    char **sp1_board = init_board();

    char **p2_board = init_board();
    printf("This is player 2 board\n");
    display_board(p2_board);

    char coord[100]; 
    int win; 
    while(flag == 0){
        printf("Input coord\n");
        fgets(coord, sizeof(coord), stdin);
        write(fd_out, coord, sizeof(coord));

        read(fd_in, p2_board, sizeof(p2_board));
        display_board(p1_board);
        display_board(p2_board);

        read(fd_in, &win, sizeof(win));
        if(win == 1){
            printf("Player 1 wins!\n");
            free_board(p1_board); 
            free_board(sp1_board);
            free_board(p2_board);
            exit(0);
        }
        
        read(fd_in, coord, sizeof(coord));
        int check = launch_missile(coord, p1_board, sp1_board, p1_ships);
        //write check value to client
        //client checks if it's 0
        //int check = 0
        //while(check == 0) check = launch_missile; write; read
        write(fd_out, sp1_board, sizeof(sp1_board));
        display_board(p1_board);
        display_board(p2_board);

        int win2 = check_win(p1_board, p1_ships);
        write(fd_out, &win2, sizeof(win2));
    }
    printf("You surrendered\n");
    close(fd_in);
    close(fd_out);
    free_board(p1_board);
    free_board(sp1_board);
    free_board(p2_board);
    exit(0);
}


int main(){
    signal(SIGINT, sighandler);
    mkfifo("output", 0666);
    mkfifo("input", 0666);
    while(1){
        handshake();
        operation();
    }
    return 0;
}
