//
// Created by hfzhou on 24-1-19.
//
#include<sys/socket.h>
#include "RUDP.h"

#ifndef RUDP_RUDP_SERVER_H
#define RUDP_RUDP_SERVER_H

// server side
bool RUDP_WaitFor(RUDP_Socket sock, short port);
RUDP_Packet PrepareSYNACK(unsigned int seq);

#endif //RUDP_RUDP_SERVER_H
