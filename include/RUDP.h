//
// Created by hfzhou on 24-1-19.
//
#include<sys/socket.h>
#include<arpa/inet.h>
#include<cstdio>
#include<cstdlib>
#include<cstring>

#ifndef RUDP_RUDP_H
#define RUDP_RUDP_H
#pragma  pack(1)

#define BODYLEN 1024

enum ConnectState
{
    uninitialized,listening,syn_send,syn_ack_send,finished,half_closed,closed
};

struct ControlFlags
{
    bool syn:1;
    bool ack:1;
    bool fin:1;
    bool preserved:5;
};

struct RUDP_Header
{
    unsigned int seq;
    unsigned int ack;
    unsigned short checksum;
    unsigned short window;
    /*
     * control:SYN,ACK,FIN
     */
    ControlFlags control;
    unsigned char len;
};


struct RUDP_Packet
{
    RUDP_Header header;
    unsigned *payload;
};

struct RUDP_Socket
{
    int socket;
    sockaddr_in addr;
    ConnectState state;
    unsigned int seq,ack;
};


void err(const char *);
void InitAddr(sockaddr_in *addr, const char *ip, const char *port);
void InitAddr(sockaddr_in *addr, const char *ip, short port);


RUDP_Socket RUDP_Init();


#pragma pack()
#endif //RUDP_RUDP_H

