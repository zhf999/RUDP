//
// Created by hfzhou on 24-1-19.
//
#include "RUDP_server.h"
#include "RUDP.h"
#include<random>

bool RUDP_WaitFor(RUDP_Socket sock, short port)
{
    sockaddr_in servAddr;
    InitAddr(&servAddr, nullptr, "7788");
    if(bind(sock.socket,(sockaddr*)&servAddr,sizeof(servAddr))==-1)
        err("Server bind error!");
    socklen_t addrLen = sizeof(sock.addr);
    RUDP_Packet rcv_syn;

    recvfrom(sock.socket,&rcv_syn,sizeof(rcv_syn),0,(sockaddr*)&sock.addr,&addrLen);

    if(rcv_syn.header.control.syn==1)
    {
        printf("Receive SYN from %s:%d\n", inet_ntoa(sock.addr.sin_addr), ntohs(sock.addr.sin_port));
        unsigned int seq = ntohl(rcv_syn.header.seq);
        RUDP_Packet syn_ack = PrepareSYNACK(seq);
        sendto(sock.socket,&syn_ack,sizeof(syn_ack),0,(sockaddr*)&sock.addr,sizeof(sock.addr));
    }
    sock.state = syn_ack_send;
    return true;
}

RUDP_Packet PrepareSYNACK(unsigned int seq)
{
    RUDP_Packet syn_ack;
    bzero(&syn_ack,sizeof(syn_ack));
    syn_ack.header.ack = htonl(seq+1);
    syn_ack.header.seq = htonl(rand() | (rand() << 15) | (rand() << 30));
    syn_ack.header.control.syn = true;
    syn_ack.header.control.ack = true;

    return  syn_ack;
}