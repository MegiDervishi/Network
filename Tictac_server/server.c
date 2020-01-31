#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "common.h"

#define B_SIZE 1024
int setup_server_socket(int portnb) {
    int sockfd;
    struct sockaddr_in dest;

    memset(&dest, 0, sizeof(struct sockaddr_in));
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        printf("Socket failed");
    }
    dest.sin_family = AF_INET;	
    dest.sin_addr.s_addr = INADDR_ANY;	
    dest.sin_port = htons(portnb);		
    if (bind(sockfd, (struct sockaddr *)&dest, sizeof(dest)) == -1)
        printf("Binding failed");
    return sockfd;
}


int check_win(char board[][3], Position * move) {
    int row = (int) move->row;
    int col = (int) move->col;

    if (board[row][0] == board[row][1] && board[row][0] == board[row][2]) {
        return 1;
    }
    if (board[0][col] == board[1][col] && board[0][col] == board[2][col]){
        return 1;
    } 
    //diagonals
    if (( (row == 0 && col == 0) || (row == 1 && col == 1) || (row == 2 && col ==2) )&& board[0][0] == board[1][1] && board[1][1] == board[2][2]){
        return 1;
    }
    if (( (row == 0 && col == 2) || (row == 1 && col == 1) || (row == 2 && col ==0) )&& board[0][2] == board[1][1] && board[1][1] == board[2][0]){
        return 1;
    }
    return 0; //no winner
}

int main(int argc, char const *argv[]) {

    if (argc < 3) {
        printf("Syntax: ./server <ipaddr> <port>\n");
        exit(1);
    }
    //Initialize
    int port = atoi(argv[2]);
    int serv_socket = setup_server_socket(port); //server socket

    //create new game
    Game newgame;
    newgame.id = 0;
    newgame.active = 0;
    newgame.nplayers = -1;
    
    Player player1 = newgame.player[0];
    Player player2 = newgame.player[1];

    struct sockaddr_in client_addr;
    socklen_t len;
    char recv_buff[MAXRCVLEN];
    memset(recv_buff, 0, MAXRCVLEN);

    //listen for player 1
    recvfrom(serv_socket, recv_buff, MAXRCVLEN, 0, (struct sockaddr *) &client_addr, &len);
    newgame.nplayers = 0;
    player1.socket = client_addr;
    player1.len_sock = len;
    char txt[] = "Welcome Player 1. You play with O\n\0";
    send_msg(TXT, serv_socket, strlen(txt), txt, &player1.socket, &player1.len_sock);
    
    //listen for player 2
    recvfrom(serv_socket, recv_buff, MAXRCVLEN,0 ,(struct sockaddr *) &client_addr, &len);
    newgame.nplayers = 1;
    player2.socket = client_addr;
    player2.len_sock = len;
    char txt2[] = "Welcome Player 2. You play with X\n\0";
    send_msg(TXT, serv_socket, strlen(txt2), txt2, &player2.socket, &player2.len_sock);
    
    //send greetings
    char txt3[] = "------ Both players have arrived.-----\n\0";
    send_msg(TXT, serv_socket, strlen(txt3), txt3, &player1.socket, &player1.len_sock);
    send_msg(TXT, serv_socket, strlen(txt3), txt3, &player2.socket, &player2.len_sock);


    //start game
    newgame.npos = 0; //filled pos
    int player_turn = 0;
    char board[3][3]=  {{' ', ' ', ' '},
                        {' ', ' ', ' '}, 
                        {' ', ' ', ' '}};

    while (!newgame.active) {
        //FYI info
        int send_length = fill_fyi(newgame.grid, newgame.npos, recv_buff);
        if (player_turn == 0){
            send_msg(FYI, serv_socket, send_length, recv_buff, &player1.socket, &player1.len_sock);
        }
        else{
            send_msg(FYI, serv_socket, send_length, recv_buff, &player2.socket, &player2.len_sock);
        }

        //Get the move from the client
        int valid_move = 0;

        while(!valid_move){ 
            //send the MYM and receive the MOV
            char msg0[] = "\0";
            if (player_turn == 0) {
                send_msg(MYM, serv_socket, 10, msg0, &player1.socket, &player1.len_sock);
                recvfrom(serv_socket, recv_buff, MAXRCVLEN,0 ,(struct sockaddr *) &player1.socket, &player1.len_sock);
            } else {
                send_msg(MYM, serv_socket, 10, msg0, &player2.socket, &player2.len_sock);
                recvfrom(serv_socket, recv_buff, MAXRCVLEN,0 ,(struct sockaddr *) &player2.socket, &player2.len_sock);
            }

            int r = (int) recv_buff[1];
            int c = (int) recv_buff[2];

            //check if it is a valid move
            if (r >= 3 || c >= 3 || r < 0||c < 0) {
                char msg[] = "------Invalid move try again!---\n\0";
                if (player_turn == 0){
                    send_msg(TXT, serv_socket, strlen(msg)-2, msg, &player1.socket, &player1.len_sock);
                }
                else {
                    send_msg(TXT, serv_socket, strlen(msg)-2, msg, &player2.socket, &player2.len_sock);
                }
                valid_move = 0; 
                //continue;
            }
            else {
                valid_move = 1;
            }
            
            //check if the place has been taken
            if (board[r][c] == ' '){
                valid_move = 1;
            } else if (valid_move != 0) {
                char msg2[] = "--Place been taken---\n\0";
                if (player_turn == 0){
                    send_msg(TXT, serv_socket, strlen(msg2)-2, msg2, &player1.socket, &player1.len_sock);
                }
                else {
                    send_msg(TXT, serv_socket, strlen(msg2)-2, msg2, &player2.socket, &player2.len_sock);
                }
                valid_move = 0;
                //continue;
            }
        }

        //Update the Position move
        Position *move = malloc(sizeof(Position));
        move->row = recv_buff[1];
        move->col = recv_buff[2];
        move->player = (char) player_turn;

        int rr = (int) move->row;
        int cc = (int) move->col;

        if (move->player == 1){
            board[rr][cc] = 'X';
        } else {
            board[rr][cc] = 'O';
        }
        newgame.grid[newgame.npos] = move;
        newgame.npos += 1;

        
        
        //check if there is a win
        if (check_win (board, move)) {
            char win[] = "---Winner Congrats!---\n\0";
            char lose[] = "----You suck----\n\0";
            if (player_turn == 0){
                send_msg(END, serv_socket, strlen(win), win, &player1.socket, &player1.len_sock);
                send_msg(END, serv_socket, strlen(lose), lose, &player2.socket, &player2.len_sock);
                newgame.active = 1;
            }
            else {
                send_msg(END, serv_socket, strlen(win), win, &player2.socket, &player2.len_sock);
                send_msg(END, serv_socket, strlen(lose), lose, &player1.socket, &player1.len_sock);
                newgame.active = 1;
            }
        }
        player_turn = abs(player_turn - 1);//switch to the other player

    }
    close(serv_socket);
    return 0;
    }
