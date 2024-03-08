//
// Created by hfzhou on 24-1-22.
//


#include "RUDP_client.h"
#include "RUDP.h"
#include <arpa/inet.h>
#include <random>
#include<cstring>

int main()
{
    srand((unsigned int)time(NULL));
    RUDP_Socket* clientSock = RUDP_Init();
    sockaddr_in addr;
    InitAddr(&addr, "192.168.177.130", "7788");
    if(-1== RUDP_SetAddr(clientSock,&addr))
    {
        err("SetAddr error!");
    }
    RUDP_Connect(clientSock);
    printf("Connection finished!\n");
    RUDP_Close(clientSock);

    return 0;
}