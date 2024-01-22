//
// Created by hfzhou on 24-1-19.
//

#include <cstring>
#include <random>
#include "RUDP_client.h"
#include "RUDP.h"



bool RUDP_Connect(RUDP_Socket sock, sockaddr_in *target) noexcept {
    if(connect(sock.socket,(sockaddr*)target,sizeof(*target))==-1)
    {
        printf("%d",errno);
        err("UDP connect error!");
    }

    RUDP_Packet syn_packet = PrepareSYN();
    send(sock.socket,&syn_packet,sizeof(syn_packet),0);

    return true;
}

RUDP_Packet PrepareSYN()
{
    RUDP_Packet SYN_packet;
    bzero(&SYN_packet,sizeof(SYN_packet));
    SYN_packet.header.control.syn = true;
    unsigned int randSeq = rand() | (rand() << 15) | (rand() << 30);
    SYN_packet.header.seq = htonl(randSeq);

    return SYN_packet;
}