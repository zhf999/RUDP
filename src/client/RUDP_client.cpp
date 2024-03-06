//
// Created by hfzhou on 24-1-19.
//

#include <cstring>
#include <unistd.h>
#include <random>
#include "RUDP_client.h"
#include "RUDP.h"

void* ClientLoop(void *thread_sock);
RUDP_Packet PrepareESSYN();
RUDP_Packet PrepareESAck(unsigned int seq, unsigned int ack);

void* ClientLoop(void *thread_sock)
{

    RUDP_Socket* rsock = (RUDP_Socket*)thread_sock;
    while(true)
    {
        //printf("Now in thread loop:\n");
        MutexWait(&rsock->outMutex);
        CheckResend(rsock);
        MutexRelease(&rsock->outMutex);
    }

}


bool RUDP_Connect(RUDP_Socket *sock) noexcept {
    RUDP_Packet syn_packet = PrepareESSYN();
    int res = 0;

    CreateThread(ClientLoop,sock);

    sock->seq_number = ntohl(syn_packet.header.seq);
    send(sock->socket,&syn_packet,sizeof(syn_packet),0);
    ListAppend(&sock->outList, MakeNode(syn_packet));
    return true;
}

RUDP_Packet PrepareESSYN()
{
    RUDP_Packet SYN_packet;
    bzero(&SYN_packet,sizeof(SYN_packet));
    SYN_packet.header.type = SYN;
    unsigned int randSeq = rand() | (rand() << 15) | (rand() << 30);
    SYN_packet.header.seq = htonl(randSeq);

    return SYN_packet;
}


