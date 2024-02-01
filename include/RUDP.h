//
// Created by hfzhou on 24-1-19.
//
#include<sys/socket.h>
#include<arpa/inet.h>
#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<sys/time.h>
#include "Queue.h"
#ifndef RUDP_RUDP_H
#define RUDP_RUDP_H

#pragma  pack(1)

#define BODYLEN 1024

enum ConnectState
{
    INIT,LISTEN,ESTABLISHED,FINISHED
};

enum PacketType
{
    DATA,SYN,ACK,WND_REQUEST,WND_INFO
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
    unsigned char len;
    PacketType type;
};


struct RUDP_Packet
{
    RUDP_Header header;
    unsigned *payload;
};


struct SendNode
{
    long send_time;
    RUDP_Packet packet;
    SendNode *next;
};

struct SendQueue
{
    int len;
    SendNode *head;
    SendNode *tail;
};

struct RUDP_Socket
{
    int socket;
    sockaddr_in addr;
    ConnectState state;
    unsigned int seq_number,ack_number;
    SendQueue sendQueue,rcvQueue,outQueue,ackQueue;
    int window;
    int mss;
    int rto; //  micro second
};


void err(const char *);
void InitAddr(sockaddr_in *addr, const char *ip, const char *port);
void InitAddr(sockaddr_in *addr, const char *ip, short port);
long CurrentTime();

void QueueInit(SendQueue *queue);
bool isQueueEmpty(SendQueue *queue);
SendNode* QueueFront(SendQueue *queue);
void QueuePush(SendQueue *queue,SendNode *node);
SendNode *MakeNode(RUDP_Packet packet);

RUDP_Socket RUDP_Init();
int RUDP_send(RUDP_Socket *rsock,void* data, unsigned long len);
void RUDP_Update(RUDP_Socket *rsock);
void RUDP_resend(RUDP_Socket *rsock);

#pragma pack()


#endif //RUDP_RUDP_H

