//
// Created by hfzhou on 24-1-19.
//
#include "RUDP_server.h"
#include "RUDP.h"
#include <unistd.h>
#include<random>

RUDP_Packet PrepareSYNACK(unsigned int seq);

bool RUDP_WaitFor(RUDP_Socket *sock, short port)
{
    sockaddr_in servAddr;
    InitAddr(&servAddr, nullptr, port);
    if(bind(sock->socket,(sockaddr*)&servAddr,sizeof(servAddr))==-1)
        err("Server bind error!");
    sock->state = LISTEN;
    socklen_t addrLen = sizeof(sock->addr);
    RUDP_Packet rcv_syn_packet;
    while(1)
    {
        int res = recvfrom(sock->socket, &rcv_syn_packet, sizeof(rcv_syn_packet),0,(sockaddr*)&sock->addr,&addrLen);
        if(res!=-1)
        {
            if(rcv_syn_packet.header.type == SYN)
            {
                unsigned int syn_seq = ntohl(rcv_syn_packet.header.seq);
                RUDP_Packet synack_packet = PrepareSYNACK(syn_seq);
                send(sock->socket,&synack_packet,sizeof(synack_packet),0);
                if(connect(sock->socket,(sockaddr*)&sock->addr,sizeof(sock->addr))==-1)
                {
                    printf("%d",errno);
                    err("UDP connect error!");
                    return -1;
                }
                QueuePush(&sock->outQueue, MakeNode(synack_packet));
                sock->state = HALF_ESTABLISHED;
                sock->seq_number = ntohl(synack_packet.header.seq) + 1;
                sock->ack_number = syn_seq + 1;
                break;
            }
        }
        usleep(1000);

    }
    return true;
}

RUDP_Packet PrepareSYNACK(unsigned int seq)
{
    RUDP_Packet syn_ack;
    bzero(&syn_ack,sizeof(syn_ack));
    syn_ack.header.ack = htonl(seq);
    syn_ack.header.seq = htonl(rand() | (rand() << 15) | (rand() << 30));
    syn_ack.header.type = ACK;

    return  syn_ack;
}