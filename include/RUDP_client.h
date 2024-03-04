//
// Created by hfzhou on 24-1-19.
//
#include<sys/socket.h>
#include "RUDP.h"

#ifndef RUDP_RUDP_CLIENT_H
#define RUDP_RUDP_CLIENT_H


// client side
bool RUDP_Connect(RUDP_Socket *sock) noexcept;

#endif //RUDP_RUDP_CLIENT_H
