//
// Created by hfzhou on 24-1-19.
//

#include <cstring>
#include <random>
#include "RUDP_client.h"
#include "RUDP.h"

RUDP_Packet PrepareSYN();
RUDP_Packet PrepareAck(unsigned int seq,unsigned int ack);


bool RUDP_Connect(RUDP_Socket sock, sockaddr_in *target) noexcept {
    if(connect(sock.socket,(sockaddr*)target,sizeof(*target))==-1)
    {
        printf("%d",errno);
        err("UDP connect error!");
    }

    RUDP_Packet syn_packet = PrepareSYN();
    send(sock.socket,&syn_packet,sizeof(syn_packet),0);
    sock.state = syn_send;
    // receive synack packet from server
    RUDP_Packet expected_synack;
    unsigned int syn_seq,syn_ack_seq,syn_ack_ack;
    do{
        recv(sock.socket,&expected_synack,sizeof(syn_packet),0);
        syn_seq = ntohl(syn_packet.header.seq);
        syn_ack_seq = ntohl(expected_synack.header.seq);
        syn_ack_ack = ntohl(expected_synack.header.ack);
    }while(syn_seq+1!=syn_ack_ack);

    // send ack packet to server
    RUDP_Packet ack_packet = PrepareAck(syn_seq,syn_ack_seq);
    send(sock.socket,&ack_packet,sizeof(ack_packet),0);
    sock.state = finished;
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

RUDP_Packet PrepareAck(unsigned int seq,unsigned int ack)
{
    RUDP_Packet ACK_packet;
    bzero(&ACK_packet,sizeof(ACK_packet));
    ACK_packet.header.control.ack = true;
    ACK_packet.header.seq = htonl(seq+1);
    ACK_packet.header.ack = htonl(ack+1);

    return ACK_packet;
}