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
void VerifyEstablishSYNACK(RUDP_Socket *, RUDP_Packet);

void* ClientLoop(void *thread_sock)
{
    RUDP_Socket* rsock = (RUDP_Socket*)thread_sock;
    while(true)
    {
        if(rsock->state==INIT)
        {
            // try to read from
            // repeatedly send the SYN packet
            CheckResend(rsock);
            RUDP_Packet rcv_syanck_packet;
            int res = recv(rsock->socket, &rcv_syanck_packet, sizeof(rcv_syanck_packet), 0);
            if(res>0 && rcv_syanck_packet.header.type == SYNACK)
            {
                VerifyEstablishSYNACK(rsock, rcv_syanck_packet);
            }
        }
        else if (rsock->state==ESTABLISHED)
        {
            CheckInput(rsock);
            CheckResend(rsock);
        }
        //printf("Now in thread loop:\n");
    }

}

void VerifyEstablishSYNACK(RUDP_Socket *rsock, RUDP_Packet temp)
{
    long ack = ntohl(temp.header.ack);
    if(ack==rsock->seq_number)
    {
        CheckACK(rsock,ack);
        MyLog(DEBUG,"BACK: Receive a SYNACK from peer. seq=%ld, ack=%ld",ntohl(temp.header.seq),ack);
        rsock->ack_number = ntohl(temp.header.seq);
        rsock->state = ESTABLISHED;
        SendACK(rsock,rsock->ack_number);
        sem_post(&rsock->mainBlock);
    }
}



bool RUDP_Connect(RUDP_Socket *sock) noexcept {
    MyLog(INFO,"Start connection.");
    RUDP_Packet syn_packet = PrepareESSYN();
    int res = 0;

    sock->pthread = CreateThread(ClientLoop,sock);

    sock->seq_number = ntohl(syn_packet.header.seq);
    MyLog(DEBUG,"Send a SYN to peer. seq=%ld",ntohl(syn_packet.header.seq));
    SendRawPacket(sock, syn_packet);
    sem_wait(&sock->mainBlock);
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


