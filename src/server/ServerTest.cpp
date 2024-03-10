//
// Created by hfzhou on 24-1-22.
//

#include "RUDP.h"
#include "RUDP_server.h"
#include <random>

int main()
{
    SetFileLogLevel(DEBUG, true);
    SetTerminalLogLevel(DEBUG);

    srand((unsigned int)time(NULL));
    RUDP_Socket *servSock = RUDP_Init();
    RUDP_WaitFor(servSock,7788);

    getchar();
    RUDP_Close(servSock);

    LogClose();

    return 0;
}