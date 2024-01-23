//
// Created by hfzhou on 24-1-19.
//

#include "RUDP.h"

void err(const char *s)
{
    printf("%s\n",s);
    exit(0);
}

void InitAddr(sockaddr_in *addr, const char *ip, const char *port) {
    int iport = atoi(port);
    memset(addr,0,sizeof(addr));
    addr->sin_family = AF_INET;
    if(ip== nullptr)
        addr->sin_addr.s_addr = INADDR_ANY;
    else
        addr->sin_addr.s_addr = inet_addr(ip);
    addr->sin_port = htons(iport);
}

void InitAddr(sockaddr_in *addr, const char *ip, short port)
{
    memset(addr,0,sizeof(addr));
    addr->sin_family = AF_INET;
    if(ip== nullptr)
        addr->sin_addr.s_addr = INADDR_ANY;
    else
        addr->sin_addr.s_addr = inet_addr(ip);
    addr->sin_port = htons(port);
}

RUDP_Socket RUDP_Init()
{
    RUDP_Socket rudpSocket;
    rudpSocket.socket = socket(AF_INET,SOCK_DGRAM,0);
    bzero(&rudpSocket.addr,sizeof(rudpSocket.addr));
    rudpSocket.state = uninitialized;
    if(rudpSocket.socket>0)
        return rudpSocket;
    else
        err("RUDP socket init error!");

    return rudpSocket;
}
