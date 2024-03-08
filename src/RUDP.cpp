//
// Created by hfzhou on 24-1-19.
//

#include <cerrno>
#include "RUDP.h"
#include "RUDP_client.h"

void err(const char *s)
{
    printf("%s\n",s);
    exit(0);
}

long CurrentTime()
{
    timeval cur;
    gettimeofday(&cur, nullptr);
    return cur.tv_sec*1000+cur.tv_usec/1000;
}

void ListInit(List *list)
{
    list->len = 0;
    list->head = new ListNode;
    list->tail = list->head;
}
bool isListEmpty(List *list)
{
    return list->head == list->tail;
}
ListNode *ListFront(List *list)
{
    if (isListEmpty(list))
        return nullptr;
    ListNode *res = list->head->next;
    list->head->next = res->next;
    res->next->prev = list->head;
    res->next = nullptr;
    res->prev = nullptr;
    list->len--;
    return res;
}
void ListAppend(List *list, ListNode *node)
{
    list->len++;
    list->tail->next = node;
    node->prev = list->tail;
    list->tail = node;
}

void ListRemove(List *list, ListNode *node)
{
    list->len--;
    ListNode *next = node->next, *prev = node->prev;
    prev->next = node->next;
    if(next!= nullptr)next->prev = prev;
    node->next = nullptr;
    node->prev = nullptr;
}

ListNode *MakeNode(RUDP_Packet packet)
{
    ListNode *node = new ListNode;
    node->send_time = CurrentTime();
    node->next = nullptr;
    node->prev = nullptr;
    node->packet = packet;
    return node;
}

void InitAddr(sockaddr_in *addr, const char *ip, const char *port) {
    int iport = atoi(port);
    memset(addr,0,sizeof(addr));
    addr->sin_family = AF_INET;
    if(ip== nullptr)
        addr->sin_addr.s_addr = INADDR_ANY;
    else
        addr->sin_addr.s_addr = inet_addr(ip);
    addr->sin_port = htons(iport);
}

void InitAddr(sockaddr_in *addr, const char *ip, short port)
{
    memset(addr,0,sizeof(addr));
    addr->sin_family = AF_INET;
    if(ip== nullptr)
        addr->sin_addr.s_addr = INADDR_ANY;
    else
        addr->sin_addr.s_addr = inet_addr(ip);
    addr->sin_port = htons(port);
}

RUDP_Socket* RUDP_Init()
{
    RUDP_Socket* rudpSocket = new RUDP_Socket;
    rudpSocket->socket = socket(AF_INET,SOCK_DGRAM|SOCK_NONBLOCK,0);
    bzero(&rudpSocket->addr,sizeof(rudpSocket->addr));
    rudpSocket->state = INIT;
    if(rudpSocket->socket<=0)
        err("RUDP socket init error!");
    struct timeval timeout={1,0};//3s
    setsockopt(rudpSocket->socket,SOL_SOCKET,SO_SNDTIMEO,(const char*)&timeout,sizeof(timeout));
    setsockopt(rudpSocket->socket,SOL_SOCKET,SO_RCVTIMEO,(const char*)&timeout,sizeof(timeout));

    ListInit(&rudpSocket->sendList);
    ListInit(&rudpSocket->rcvList);
    ListInit(&rudpSocket->outList);
    ListInit(&rudpSocket->ackList);
    rudpSocket->window = 32;
    rudpSocket->mss = 1024;
    rudpSocket->bufferContentSize = 0;
    rudpSocket->rto = 1000;

    MutexInit(&rudpSocket->sendMutex);
    MutexInit(&rudpSocket->outMutex);
    MutexInit(&rudpSocket->rcvMutex);
    MutexInit(&rudpSocket->ackMutex);

    sem_init(&rudpSocket->mainBlock,0,0);
    return rudpSocket;
}

void RUDP_Close(RUDP_Socket* rsock)
{
    int res = pthread_cancel(rsock->pthread);
    if(res!=0)
        err("Cancel thread fail!\n");
    pthread_join(rsock->pthread, nullptr);
    delete rsock;
}


int RUDP_SetAddr(RUDP_Socket *sock, sockaddr_in *addr)
{
    if(connect(sock->socket,(sockaddr*)addr,sizeof(*addr))==-1)
    {
        printf("%d",errno);
        err("UDP connect error!");
        return -1;
    }

    sock->addr = *addr;

    return 1;
}


int RUDP_send(RUDP_Socket *rsock, char *data, unsigned long len)
{
    if(rsock->state == INIT)
    {
        if(RUDP_Connect(rsock)==false)
        {
            err("Connect error");
        }
    }
    char *start = data;
    long restLen = len;

    while(restLen>0)
    {
        RUDP_Packet newPacket;
        newPacket.header.type = DATA;
        newPacket.header.seq = htonl(rsock->seq_number);
        rsock->seq_number += 1;

        unsigned long payLoadLen = restLen>rsock->mss?rsock->mss:restLen;

        newPacket.header.len = payLoadLen;
        memcpy(newPacket.payload,start,payLoadLen);
        start += payLoadLen;
        restLen -= payLoadLen;

        ListNode * newNode = MakeNode(newPacket);
        ListAppend(&rsock->sendList, newNode);
    }

    RUDP_Update(rsock);
    return 1;
}

int RUDP_recv(RUDP_Socket *rsock, char *data, unsigned long len)
{
    long restLen = len;
    while (restLen>0)
    {
        if(restLen<=rsock->bufferContentSize)
        {
            memcpy(data,rsock->rcvBuffer,restLen);
            memcpy(rsock->rcvBuffer,rsock->rcvBuffer+len+1,restLen);
            rsock->bufferContentSize -= restLen;
            restLen = 0;
            return len;
        }
        else
        {
            int readLen = rsock->bufferContentSize;
            memcpy(data,rsock->rcvBuffer,readLen);
            memcpy(rsock->rcvBuffer,rsock->rcvBuffer+len+1,readLen);
            rsock->bufferContentSize = 0;
            restLen = 0;
            return readLen;
            // TODO nonblock
//            restLen -= readLen;
//            RUDP_Update(rsock);
        }
    }
}



void RUDP_Update(RUDP_Socket *rsock)
{
    RUDP_Resend(rsock);
    RUDP_Flush(rsock);
    RUDP_PickPacket(rsock);
}

void RUDP_Resend(RUDP_Socket *rsock)
{
    long curtime = CurrentTime();
    for(ListNode *cur=rsock->outList.head->next; cur != nullptr; cur=cur->next)
    {
        if(curtime-cur->send_time>rsock->rto)
        {
            cur->send_time = curtime;
            send(rsock->socket,&cur->packet,sizeof(cur->packet),0);
        }
    }
}

void RUDP_Flush(RUDP_Socket *rsock)
{
    while(!isListEmpty(&rsock->sendList))
    {
        ListNode* front = ListFront(&rsock->sendList);
        front->send_time = CurrentTime();
        send(rsock->socket,&front->packet,sizeof(front->packet),0);
        ListAppend(&rsock->outList, front);
    }
}

void RUDP_PickPacket(RUDP_Socket *rsock)
{
    bool success;
    do{
        success = false;
        for(ListNode *cur=rsock->rcvList.head->next; cur != nullptr; cur=cur->next)
        {
            if(rsock->ack_number==cur->packet.header.seq)
            {
                if(rsock->bufferContentSize+cur->packet.header.len<=BUFFERLEN)
                {
                    memcpy(rsock->rcvBuffer+rsock->bufferContentSize,cur->packet.payload,
                           cur->packet.header.len);
                    rsock->bufferContentSize += cur->packet.header.len;
                    success =true;
                }
            }
        }
    } while (success);
}

void CheckResend(RUDP_Socket *rsock)
{
    MutexWait(&rsock->outMutex);
    for(ListNode *cur=rsock->outList.head->next;cur!= nullptr;cur=cur->next)
    {
        long cur_time = CurrentTime();
        if(cur_time-cur->send_time>rsock->rto)
        {
            cur->send_time = cur_time;
            send(rsock->socket,&cur->packet,sizeof(cur->packet),0);
            printf("Resend packet!\n");
        }
    }
    MutexRelease(&rsock->outMutex);
}

void CheckInput(RUDP_Socket *rsock)
{
    RUDP_Packet temp;
    int res = recv(rsock->socket, &temp, sizeof(temp), 0);
    if(res>0)
    {
        PacketType type = temp.header.type;
        if(type==DATA)
        {

        }
        else if(type==ACK)
        {

        }
        // TODO finish more selection for input packet
    }
}

void SendACK(RUDP_Socket *rsock,long ack)
{
    RUDP_Packet packet;
    bzero(&packet, sizeof(packet));
    packet.header.len = 0;
    packet.header.type = ACK;
    packet.header.ack = htonl(ack);
    send(rsock->socket,&packet,sizeof(packet),0);
}
