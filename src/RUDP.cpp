//
// Created by hfzhou on 24-1-19.
//

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

void QueueInit(SendQueue *queue)
{
    queue->len = 0;
    queue->head = new SendNode;
    queue->tail = queue->head;
}
bool isQueueEmpty(SendQueue *queue)
{
    return queue->head == queue->tail;
}
SendNode *QueueFront(SendQueue *queue)
{
    if (isQueueEmpty(queue))
        return nullptr;
    SendNode *res = queue->head->next;
    queue->head = queue->head->next;
    res->next = nullptr;
    return res;
}
void QueuePush(SendQueue *queue,SendNode *node)
{
    queue->tail->next = node;
}

SendNode *MakeNode(RUDP_Packet packet)
{
    SendNode *node = new SendNode;
    node->send_time = CurrentTime();
    node->next = nullptr;
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

RUDP_Socket RUDP_Init()
{
    RUDP_Socket rudpSocket;
    rudpSocket.socket = socket(AF_INET,SOCK_DGRAM|SOCK_NONBLOCK,0);
    bzero(&rudpSocket.addr,sizeof(rudpSocket.addr));
    rudpSocket.state = INIT;
    if(rudpSocket.socket<=0)
        err("RUDP socket init error!");
    //struct timeval timeout={3,0};//3s
    //setsockopt(rudpSocket.socket,SOL_SOCKET,SO_SNDTIMEO,(const char*)&timeout,sizeof(timeout));
    //setsockopt(rudpSocket.socket,SOL_SOCKET,SO_RCVTIMEO,(const char*)&timeout,sizeof(timeout));

    QueueInit(&rudpSocket.sendQueue);
    QueueInit(&rudpSocket.rcvQueue);
    rudpSocket.window = 32;
    return rudpSocket;
}



int RUDP_send(RUDP_Socket *rsock,void* data, unsigned long len)
{
    if(rsock->state == INIT)
    {
        if(RUDP_Connect(rsock)==false)
        {
            err("Connect error");
        }
    }
    // TODO
}

void RUDP_Update(RUDP_Socket *rsock)
{
    RUDP_resend(rsock);
    // TODO
}

void RUDP_resend(RUDP_Socket *rsock)
{
    long curtime = CurrentTime();
    for(SendNode *cur=rsock->outQueue.head->next;cur!= nullptr;cur=cur->next)
    {
        if(curtime-cur->send_time>rsock->rto)
        {
            cur->send_time = curtime;
            send(rsock->socket,&cur->packet,sizeof(cur->packet),0);
        }
    }
}


