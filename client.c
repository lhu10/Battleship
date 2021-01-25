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
        flag = 1; 
    }
}
void check_error(int check){
    if(check == -1){
        printf("Error %d: %s\n", errno, strerror(errno));
    }
}

void handshake(){
    char private[256], message[100];
    sprintf(private, "%d", getpid());
    mkfifo(private, 0666);
    
    int fd_write, fd_read, check; 
    printf("Client connecting to server\n");
    fd_write = open("public", O_WRONLY);
    check_error(fd_write);
    //client sends message to server
    check = write(fd_write, private, strlen(private)+1);
    check_error(check);

    printf("Client receiving server's message: ");
    fd_read = open(private, O_RDONLY);
    check_error(fd_read);
    //client receives server's message
    check = read(fd_read, message, sizeof(message));
    check_error(check);
    printf("%s\n", message);
    printf("Client removing private FIFO\n");
    unlink(private);
    
    //client sends response to server
    char response[50] = "Handshake Completed";
    check = write(fd_write, response, sizeof(response)); 
    check_error(check);

    close(fd_read);
    close(fd_write);

}

void operation(){
    int fd_in, fd_out;
    fd_out = open("input", O_WRONLY);
    fd_in = open("output", O_RDONLY); 
    int gen_check2;
    read(fd_in, &gen_check2, sizeof(gen_check2));
    if(gen_check2 == 0){
        printf("Error! Player 2 board generation failed\n");
        exit(0);
    }
    char **p1_board = init_board(); 
    struct ship_node p1_ships[MAXSHIPS];
    int gen_check = generate_board("player1", p1_board, p1_ships);
    write(fd_out, &gen_check, sizeof(gen_check));
    if(gen_check == 0){
        free_board(p1_board);
        exit(0);
    }
    printf("This is player 1 board\n");
    display_board(p1_board);
    
    printf("\n");
    //copy of p1_board without visible ships to be sent to server and displayed by server
    char **sp1_board = init_board();
    char **p2_board = init_board();
    printf("This is player 2 board\n");
    display_board(p2_board);

    printf("---------------------------------------\n");
    printf("Player 1 - Your Turn\n");
    printf("---------------------------------------\n");
    char coord[256]; 
    int win, win2; 
    char *sp1_arr;
    char player[100] = "Player 1 (you)";
    while(flag == 0){
        printf("Input coord:\n");
        fgets(coord, sizeof(coord), stdin);
        //surrender check
        int flag_check = flag; 
        write(fd_out, &flag_check, sizeof(flag));
        if(flag_check == 1){
            break;
        }

        write(fd_out, coord, sizeof(coord));
        //check if coord is valid
        //if invalid, input another coord
        int check2 = 0; 
        while(check2 == 0){
            read(fd_in, &check2, sizeof(check2));
            if(check2 == 0){
                *strchr(coord, '\n') = 0;   //gets rid of new line
                printf("%s is an invalid point\n", coord);
                printf("Input another coord:\n");
                fgets(coord, sizeof(coord), stdin);
                write(fd_out, coord, sizeof(coord));
            }
        }

        printf("---------------------------------------\n");
        printf("Player 1 (you) launched a missile!\n");
        printf("---------------------------------------\n");
        //server sent 1d array to client
        char p2_arr[101]; 
        read(fd_in, p2_arr, sizeof(p2_arr));
        //convert 1d to 2d array so it can display
        one_to_two(p2_arr, p2_board);
        
        printf("player1 (you)\n");
        display_board(p1_board);
        printf("player2\n");
        display_board(p2_board);

        read(fd_in, &win, sizeof(win));
        if(win == 1){
            printf("You cleared all the ships!\n");
            printf("Player 1 wins!\n");
            end_prog(sp1_arr, p1_board, sp1_board, p2_board);
        }

        printf("---------------------------------------\n");
        printf("Waiting for Player 2 to make a move...\n");
        printf("---------------------------------------\n");
        //surrender check
        int flag_check2; 
        read(fd_in, &flag_check2, sizeof(flag_check2));
        if(flag_check2 == 1){
            printf("You won!\n");
            strcpy(player, "Player 2");
            flag = 1; 
            break;
        }

        read(fd_in, coord, sizeof(coord));
        //check if coord is valid
        int check = 0; 
        while(check == 0){
            check = launch_missile(coord, p1_board, sp1_board, p1_ships);
            write(fd_out, &check, sizeof(check));
            if(check == 0){
                read(fd_in, coord, sizeof(coord));
            }
        }

        printf("---------------------------------------\n");
        printf("Player 2 launched a missile!\n");
        printf("---------------------------------------\n");
        //can't send 2d array through read and write but can send 1d array
        //convert 2d to 1d array
        sp1_arr = two_to_one(sp1_board);
        char sp1_arr2[101];
        memset(sp1_arr2, 0, sizeof(sp1_arr2));
        strncpy(sp1_arr2, sp1_arr, 100);
        write(fd_out, sp1_arr2, sizeof(sp1_arr2));

        printf("player1 (you)\n");
        display_board(p1_board);
        printf("player2\n");
        display_board(p2_board);

        win2 = check_win(p1_board, p1_ships);
        write(fd_out, &win2, sizeof(win2));
        if(win2 == 1){
            printf("All of your ships were cleared!\n");
            printf("Player 1 loses!\n");
            end_prog(sp1_arr, p1_board, sp1_board, p2_board);
        }
    }
    printf("%s surrendered\n", player);
    close(fd_in);
    close(fd_out);
    end_prog(sp1_arr, p1_board, sp1_board, p2_board);
}

int main(){
    signal(SIGINT, sighandler);
    handshake();   
    operation();
    return 0;
}
