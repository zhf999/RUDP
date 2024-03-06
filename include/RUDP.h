//
// Created by hfzhou on 24-1-19.
//
#include<sys/socket.h>
#include<arpa/inet.h>
#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<sys/time.h>
#include "utils.h"
#ifndef RUDP_RUDP_H
#define RUDP_RUDP_H

#pragma  pack(1)

#define BODYLEN 1024
#define BUFFERLEN 1024*1024

enum ConnectState
{
    INIT,LISTEN,ESTABLISHED,HALF_ESTABLISHED,FINISHED
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
    unsigned payload[BODYLEN];
};


struct ListNode
{
    long send_time;
    RUDP_Packet packet;
    ListNode *next,*prev;
};

struct List
{
    int len;
    ListNode *head;
    ListNode *tail;
};

struct RUDP_Socket
{
    int socket;
    sockaddr_in addr;
    ConnectState state;
    unsigned int seq_number; // the last number to send
    unsigned int ack_number; // the next seq number to receive i.e. the
    List sendList,rcvList,outList,ackList;
    pthread_mutex_t sendMutex,outMutex,rcvMutex,ackMutex;

    char rcvBuffer[BUFFERLEN];
    unsigned int bufferContentSize;

    int window;
    int mss;
    int rto; //  micro second
};


void err(const char *);
void InitAddr(sockaddr_in *addr, const char *ip, const char *port);
void InitAddr(sockaddr_in *addr, const char *ip, short port);
long CurrentTime();

void ListInit(List *list);
bool isListEmpty(List *list);
ListNode* ListFront(List *list);
void ListAppend(List *list, ListNode *node);
ListNode *MakeNode(RUDP_Packet packet);
void ListRemove(List *list, ListNode *node);

RUDP_Socket* RUDP_Init();
void RUDP_Close(RUDP_Socket*);
int RUDP_SetAddr(RUDP_Socket *sock, sockaddr_in *addr);
int RUDP_send(RUDP_Socket *rsock, char *data, unsigned long len);
int RUDP_recv(RUDP_Socket *rsock, char *data, unsigned long len);
void RUDP_Update(RUDP_Socket *rsock);
void RUDP_Flush(RUDP_Socket *rsock);
void RUDP_Resend(RUDP_Socket *rsock);
void RUDP_PickPacket(RUDP_Socket *rsock);

void CheckResend(RUDP_Socket *rsock);

#pragma pack()


#endif //RUDP_RUDP_H

