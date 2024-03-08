//
// Created by hfzhou on 24-1-19.
//

#include <cstring>
#include <unistd.h>
#include <random>
#include "RUDP_client.h"
#include "RUDP.h"

void* ClientLoop(void *thread_sock);
RUDP_Packet PrepareESSYN();
RUDP_Packet PrepareESAck(unsigned int seq, unsigned int ack);

void* ClientLoop(void *thread_sock)
{

    RUDP_Socket* rsock = (RUDP_Socket*)thread_sock;
    while(true)
    {
        if(rsock->state==INIT)
        {
            // try to read from
            // repeatedly send the SYN packet
            CheckResend(rsock);
            RUDP_Packet temp;
            int res = recv(rsock->socket,&temp,sizeof(temp),0);
            if(res>0)
            {
                if(temp.header.type==ACK)
                {
                    long ack = ntohl(temp.header.ack);
                    for(ListNode *p=rsock->outList.head->next;p!= nullptr;p=p->next)
                        if(htonl(p->packet.header.seq)==ack)
                            ListRemove(&rsock->outList,p);
                    if(ack==rsock->seq_number)
                    {
                        printf("Receive SYN ACK packet!\n");
                        rsock->ack_number = ntohl(temp.header.seq);
                        SendACK(rsock,rsock->ack_number);
                        sem_post(&rsock->mainBlock);
                    }
                }
            }
        }
        //printf("Now in thread loop:\n");
    }

}


bool RUDP_Connect(RUDP_Socket *sock) noexcept {
    printf("Start Connection\n");
    RUDP_Packet syn_packet = PrepareESSYN();
    int res = 0;

    sock->pthread = CreateThread(ClientLoop,sock);

    sock->seq_number = ntohl(syn_packet.header.seq);
    send(sock->socket,&syn_packet,sizeof(syn_packet),0);
    printf("Send SYN\n");
    ListAppend(&sock->outList, MakeNode(syn_packet));
    sem_wait(&sock->mainBlock);
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


