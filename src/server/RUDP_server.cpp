//
// Created by hfzhou on 24-1-19.
//
#include "RUDP_server.h"
#include "RUDP.h"
#include <unistd.h>
#include<random>

void* ServerLoop(void* thread_sock);
void ServerRcvSYN(RUDP_Socket*);
RUDP_Packet PrepareSYNACK(unsigned int seq);
void VerifyEstablishACK(RUDP_Socket* ,RUDP_Packet );

void* ServerLoop(void* thread_sock)
{
    RUDP_Socket *rsock = (RUDP_Socket*)thread_sock;
    while(1)
    {
        if(rsock->state==LISTEN)
        {
            ServerRcvSYN(rsock);
        }
        else if(rsock->state==HALF_ESTABLISHED)
        {
            RUDP_Packet rcv_ack;
            int res = recv(rsock->socket,&rcv_ack,sizeof(rcv_ack),0);
            if(res>0)
            {
                VerifyEstablishACK(rsock, rcv_ack);
            }
            else CheckResend(rsock);
        }
        else if(rsock->state==ESTABLISHED)
        {
            CheckInput(rsock);
            CheckResend(rsock);
        }
    }
}

void VerifyEstablishACK(RUDP_Socket *rsock, RUDP_Packet rcv_ack)
{
    if(rcv_ack.header.type==ACK)
    {
        long ack_number = ntohl(rcv_ack.header.ack);
        if(ack_number==rsock->seq_number)
        {
            MyLog(DEBUG,"BACK: receive ACK from peer, ack=%ld",ack_number);
            rsock->state=ESTABLISHED;
            sem_post(&rsock->mainBlock);
        }
    }
}



void ServerRcvSYN(RUDP_Socket* rsock)
{
    socklen_t addrLen = sizeof(rsock->addr);
    RUDP_Packet rcv_syn_packet;
    int res = recvfrom(rsock->socket, &rcv_syn_packet, sizeof(rcv_syn_packet),0,(sockaddr*)&rsock->addr,&addrLen);
    if(res!=-1&&rcv_syn_packet.header.type==SYN)
    {
        MyLog(DEBUG,"BACK: receive SYN from peer, seq=%ld",ntohl(rcv_syn_packet.header.seq));
        if(connect(rsock->socket,(sockaddr*)&rsock->addr,sizeof(rsock->addr))==-1)
        {
            printf("%d",errno);
            err("UDP connect error!");
        }
        unsigned int syn_seq = ntohl(rcv_syn_packet.header.seq);
        RUDP_Packet synack_packet = PrepareSYNACK(syn_seq);
        SendRawPacket(rsock,synack_packet);
        MyLog(DEBUG,"BACK: send ACK to peer, seq=%ld ack=%ld", ntohl(synack_packet.header.seq),
                                                         ntohl(synack_packet.header.ack));
        rsock->state = HALF_ESTABLISHED;
        rsock->seq_number = ntohl(synack_packet.header.seq);
        rsock->ack_number = syn_seq;
    }
}


bool RUDP_WaitFor(RUDP_Socket *sock, short port)
{
    sockaddr_in servAddr;
    InitAddr(&servAddr, nullptr, port);

    MyLog(INFO,"Server start listening at port %d",port);
    if(bind(sock->socket,(sockaddr*)&servAddr,sizeof(servAddr))==-1)
        err("Server bind error!");
    sock->state = LISTEN;
    sock->pthread = CreateThread(ServerLoop,sock);
    sem_wait(&sock->mainBlock);
}

RUDP_Packet PrepareSYNACK(unsigned int seq)
{
    RUDP_Packet syn_ack;
    bzero(&syn_ack,sizeof(syn_ack));
    syn_ack.header.ack = htonl(seq);
    syn_ack.header.seq = htonl(rand() | (rand() << 15) | (rand() << 30));
    syn_ack.header.type = SYNACK;

    return  syn_ack;
}