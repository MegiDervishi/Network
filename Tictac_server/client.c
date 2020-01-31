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


int main(int argc, char** argv) {

    int terminate = 0;
    if (argc < 3) {
        printf("Syntax: ./client <ipaddr> <port>\n");
        exit(1);
    }

    //Initialize
    int port = atoi(argv[2]);
    char* addr = argv[1];
    struct sockaddr_in dest;
    int sockfd;
    char* msg;

    // Socket creation
    memset(&dest, 0, sizeof(struct sockaddr_in));
    inet_pton(AF_INET, argv[1], &(dest.sin_addr));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(port);
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd == -1)
            printf("Socket failed");
    dest.sin_addr.s_addr = inet_addr(addr);
    
    // Greetings message
    msg = "Hello piece of annoying server\n";
    if(sendto(sockfd , msg , strlen(msg) , 0,(const struct sockaddr *)&dest, sizeof(dest)) < 0){
        printf("Send failed");
        return 1;
    }
    puts("Greeting msg sent\n");

    //start game
    while(!terminate) {

        //printf("Start game\n");

        char board[3][3]=  {{' ', ' ', ' '},
                            {' ', ' ', ' '}, 
                            {' ', ' ', ' '}};

        //receive from server
        char buffer[B_SIZE];
        socklen_t len;
        recvfrom(sockfd, buffer, B_SIZE ,0,(struct sockaddr *)&dest, &len);
        //show_bytes(buffer,len);

        if (buffer[0] == MYM) {
            printf("Make your move!\n <row> <col>\n");
            int r = 0; int c = 0;
            fscanf(stdin, "%d %d", &r, &c);
            //send packet
            buffer[0] = MOV;
            buffer[1] = (char) r;
            buffer[2] = (char) c;
            sendto(sockfd, buffer, 3, 0,(const struct sockaddr *)&dest, sizeof(dest));

        } else if (buffer[0] == FYI) {
            printf("You have an FYI msg\n");
            int numb = (int) buffer[1];
            printf("You have %d filled position(s)\n", numb);
            //decode package
            for (int i =0; i <buffer[1]; i ++){
            int col = (int) buffer[2+3*i+1];
            int row = (int) buffer[2 + 3*i + 2];
            board[row][col] = (buffer[2 + 3*i] == 1)? 'X': 'O';
            }
            //print board
            printf("  %c |  %c |  %c \n", board[0][0], board[0][1], board[0][2]);
            printf("----+----+----\n");
            printf("  %c |  %c |  %c \n", board[1][0], board[1][1], board[1][2]);
            printf("----+----+----\n");
            printf("  %c |  %c |  %c \n", board[2][0], board[2][1], board[2][2]);

        } else if (buffer[0] == END) {
            printf("Game finished\n");
            puts(buffer+1);
            break;
        } else if (buffer[0] == TXT) {
            printf("You have a msg\n");
            printf("%s", buffer+1);
        } else {
            printf("huh!\n");
            return -404;
        }
    }
    printf("Game over\n");
    close(sockfd);

    return 0;
}