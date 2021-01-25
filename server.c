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
    fd_out = open("input", O_RDONLY);
    fd_in = open("output", O_WRONLY);

    char **p2_board = init_board();
    struct ship_node p2_ships[MAXSHIPS];
    int gen_check = generate_board("player2", p2_board, p2_ships);
    write(fd_in, &gen_check, sizeof(gen_check));
    if(gen_check == 0){
        free_board(p2_board);
        exit(0);
    }
    int gen_check2;
    read(fd_out, &gen_check2, sizeof(gen_check2));
    if(gen_check2 == 0){
        printf("Error! Player 1 board generation failed\n");
        free_board(p2_board);
        exit(0);
    }
    printf("This is player 2 board\n");
    display_board(p2_board);

    printf("\n");

    char **sp2_board = init_board();
    char **p1_board = init_board();
    printf("This is player 1 board\n");
    display_board(p1_board);

    printf("---------------------------------------\n");
    printf("Waiting for Player 1 to make a move...\n");
    printf("---------------------------------------\n");

    char coord[256];
    int win, win2; 
    char *sp2_arr;
    char player[100] = "Player 2 (you)";
    while (flag == 0){
        //surrender check
        int flag_check2; 
        read(fd_out, &flag_check2, sizeof(flag_check2));
        if(flag_check2 == 1){
            printf("You won!\n");
            strcpy(player, "Player 1");
            flag = 1; 
            break;
        }

        read(fd_out, coord, sizeof(coord));
        //check if coord is valid
        int check = 0; 
        while(check == 0){
            check = launch_missile(coord, p2_board, sp2_board, p2_ships);
            write(fd_in, &check, sizeof(check));
            if(check == 0){
                read(fd_out, coord, sizeof(coord));
            }
        }

        printf("---------------------------------------\n");
        printf("Player 1 launched a missile!\n");
        printf("---------------------------------------\n");
        //can't send 2d array through read and write but can send 1d array
        //convert 2d to 1d array
        sp2_arr = two_to_one(sp2_board);
        char sp2_arr2[101];
        memset(sp2_arr2, 0, sizeof(sp2_arr2));
        strncpy(sp2_arr2, sp2_arr, 100);
        write(fd_in, sp2_arr2, sizeof(sp2_arr2));

        printf("player2 (you)\n");
        display_board(p2_board); 
        printf("player1\n");
        display_board(p1_board);

        win = check_win(p2_board, p2_ships);
        write(fd_in, &win, sizeof(win));
        if(win == 1){
            printf("All of your ships were cleared!\n");
            printf("Player 2 loses!\n");
            end_prog(sp2_arr, p1_board, sp2_board, p2_board);
        }
        printf("---------------------------------------\n");
        printf("Player 2 - Your Turn\n");
        printf("---------------------------------------\n");
        
        printf("Input coord:\n");
        fgets(coord, sizeof(coord), stdin);
        //surrender check
        int flag_check = flag; 
        write(fd_in, &flag_check, sizeof(flag));
        if(flag_check == 1){
            break;
        }
        write(fd_in, coord, sizeof(coord));

        //check if coord is valid
        //if invalid, input another coord
        int check2 = 0; 
        while(check2 == 0){
            read(fd_out, &check2, sizeof(check2));
            if(check2 == 0){
                *strchr(coord, '\n') = 0;   //gets rid of new line
                printf("%s is an invalid point\n", coord);
                printf("Input another coord:\n");
                fgets(coord, sizeof(coord), stdin);
                write(fd_in, coord, sizeof(coord));
            }
        }

        printf("---------------------------------------\n");
        printf("Player 2 (you) launched a missile!\n");
        printf("---------------------------------------\n");
        //client sent 1d array to server
        char p1_arr[101]; 
        read(fd_out, p1_arr, sizeof(p1_arr));
        //convert 1d to 2d array so it can display
        one_to_two(p1_arr, p1_board);

        printf("player2 (you)\n");
        display_board(p2_board);
        printf("player1\n");
        display_board(p1_board);

        read(fd_out, &win2, sizeof(win2)); 
        if(win2 == 1){
            printf("You cleared all the ships!\n");
            printf("Player 2 wins!\n");
            end_prog(sp2_arr, p1_board, sp2_board, p2_board);
        }
        printf("---------------------------------------\n");
        printf("Waiting for Player 1 to make a move...\n");
        printf("---------------------------------------\n");
    }
    printf("%s surrendered\n", player);
    close(fd_in);
    close(fd_out);
    end_prog(sp2_arr, p1_board, sp2_board, p2_board);
}


int main(){
    signal(SIGINT, sighandler);
    mkfifo("output", 0666);
    mkfifo("input", 0666);
    
    handshake();
    operation();

    return 0;
}
