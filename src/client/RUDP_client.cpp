//
// Created by hfzhou on 24-1-19.
//

#include <cstring>
#include <unistd.h>
#include <random>
#include "RUDP_client.h"
#include "RUDP.h"

RUDP_Packet PrepareESSYN();
RUDP_Packet PrepareESAck(unsigned int seq, unsigned int ack);


bool RUDP_Connect(RUDP_Socket *sock) noexcept {
    RUDP_Packet syn_packet = PrepareESSYN();
    int res = 0;
    RUDP_Packet rcv_synack_packet;
    while(1)
    {
        send(sock->socket,&syn_packet,sizeof(syn_packet),0);
        res = recv(sock->socket,&rcv_synack_packet,sizeof(rcv_synack_packet),0);
        if(res!=-1)
        {
            unsigned int seq = ntohl(syn_packet.header.seq);
            unsigned int ack = ntohl(rcv_synack_packet.header.ack);
            if(rcv_synack_packet.header.type==ACK&&ack==seq)
            {
                sock->state = ESTABLISHED;
                sock->seq_number = seq + 1;
                sock->ack_number = ack + 1;
                break;
            }
        }
        else
        {
            usleep(1000);
        }
    }
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


