#include <netdb.h>
#include <sys/socket.h>

typedef struct Position {
    char player;
    char col;
    char row;
} Position;

typedef struct Player {
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];
    struct sockaddr_in socket;
    socklen_t len_sock;
} Player;

typedef struct Game {
    int id;
    int active;         // 1=yes, 0=no
    struct Player player[2];    // playing with X and O
    int turn;             // 0: waiting for players, 1: waiting for player1 msg, 2: waiting for player2 msg
    int nplayers;
    
    struct Position* grid[9]; // max nbr of positions is 9
    int npos;
} Game;

#define FYI 0x01
#define MYM 0x02
#define END 0x03
#define TXT 0x04
#define MOV 0x05
#define LFT 0x06
#define INV 0x07

#define MAXRCVLEN 500

void show_bytes(char* buffer, int len);

void send_msg(char type, int dest, int length, char* payload, struct sockaddr_in* sockstr, socklen_t* len_sock);

int pktlen(int type, int* params);

int fill_mov(char col, char row, char* p);
int fill_fyi(struct Position** grid, int npos, char* p);
int fill_end(unsigned char winner, char* p);

int sockaddr_cmp(struct sockaddr *x, struct sockaddr *y);
