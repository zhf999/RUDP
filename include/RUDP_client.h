//
// Created by hfzhou on 24-1-19.
//
#include<sys/socket.h>
#include "RUDP.h"

#ifndef RUDP_RUDP_CLIENT_H
#define RUDP_RUDP_CLIENT_H


// client side
bool RUDP_Connect(RUDP_Socket sock, sockaddr_in *target) noexcept;

RUDP_Packet PrepareSYN();
RUDP_Packet PrepareSYNACK();


#endif //RUDP_RUDP_CLIENT_H
