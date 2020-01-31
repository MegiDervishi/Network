#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

#include <sys/un.h>
#include <string.h>

#include<assert.h>        //for sockaddr_cmp

#include "common.h"

char* msgtype[] = {"FYI", "MYM", "END", "TXT", "MOV"};

void show_bytes(char* buffer, int len) {
    for (int i=0; i<len; i++) {
        printf("%3d ", (int)buffer[i]);
    }
    printf("\n");
}

void send_msg(char type, int dest, int length, char* payload, struct sockaddr_in* sockstr, socklen_t* len_sock) {
    int size_msg = length + sizeof(char); //sizeof(char)=1
    char* msg = malloc(size_msg);
    char* ite = msg;
    
    *ite = type; // FYI MYM END etc
    ite++;
    memcpy(ite, payload, length);
    
    // Specific UDP
    sendto(dest, (const char *)msg, size_msg, 0, (const struct sockaddr *) sockstr, *len_sock);
    
    free(msg);
    printf("     [t] [%3s] (to %d) \n", msgtype[type-1], dest);
}

int pktlen(int type, int* params) {
    switch(type) {
        case FYI:
            // npos = params[0]
            if (!params) printf("Error, missing parameter in pktlen FYI.\n");
            return sizeof(char) + params[0] * 3 * sizeof(char);
        case MOV:
            return 2*sizeof(char);
        case END:
            return 1*sizeof(char);
        default:
            printf("Error, unknown type of packet");
            return -1;
    }
}

int fill_end(unsigned char winner, char* p) {
    int length = pktlen(END, NULL); //1*sizeof(char);
    char *ite = p;
    *ite = winner;
    
    return length;
}

int fill_fyi(struct Position** grid, int npos, char* p) {
    int length = pktlen(FYI, &npos); //sizeof(char) + npos * 3 * sizeof(char);
    char* it = p;
    *it = (char) npos;
    it++;
    for (int i = 0; i < npos; i ++) {
        memcpy(it, &(grid[i]->player), sizeof(char));
        memcpy(it+1, &(grid[i]->col), sizeof(char));
        memcpy(it+2, &(grid[i]->row), sizeof(char));
        
        it += 3*sizeof(char); //sizeof(Position);
    }
    
    return length;
}

int fill_mov(char col, char row, char* p) {
    int length = pktlen(MOV, NULL); //2*sizeof(char);
    char *ite = p;
    *ite = col;
    ite++;
    *ite = row;
    
    return length;
}

/*
 * Copyright (c) 2014 Kazuho Oku
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

int sockaddr_cmp(struct sockaddr *x, struct sockaddr *y)
{
#define CMP(a, b) if (a != b) return a < b ? -1 : 1
    CMP(x->sa_family, y->sa_family);
    if (x->sa_family == AF_UNIX) {
        struct sockaddr_un *xun = (void*)x, *yun = (void*)y;
        int r = strcmp(xun->sun_path, yun->sun_path);
        if (r != 0)
            return r;
    } else if (x->sa_family == AF_INET) {
        struct sockaddr_in *xin = (void*)x, *yin = (void*)y;
        CMP(ntohl(xin->sin_addr.s_addr), ntohl(yin->sin_addr.s_addr));
        CMP(ntohs(xin->sin_port), ntohs(yin->sin_port));
    } else if (x->sa_family == AF_INET6) {
        struct sockaddr_in6 *xin6 = (void*)x, *yin6 = (void*)y;
        int r = memcmp(xin6->sin6_addr.s6_addr, yin6->sin6_addr.s6_addr, sizeof(xin6->sin6_addr.s6_addr));
        if (r != 0)
            return r;
        CMP(ntohs(xin6->sin6_port), ntohs(yin6->sin6_port));
        CMP(xin6->sin6_flowinfo, yin6->sin6_flowinfo);
        CMP(xin6->sin6_scope_id, yin6->sin6_scope_id);
    } else {
        assert(!"unknown sa_family");
    }
#undef CMP
    return 0;
}
