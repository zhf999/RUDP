//
// Created by hfzhou on 24-3-9.
//
//
// Created by hfzhou on 24-1-22.
//


#include "RUDP_client.h"
#include "RUDP.h"

int main()
{
    SetFileLogLevel(DEBUG, true);
    SetTerminalLogLevel(INFO);

    MyLog(INFO,"I have %d pens",5);
    MyLog(INFO,"I lost a pen");
    MyLog(DEBUG,"Only seen in files %s","Client");
    MyLog(ERROR,"I'm angry!!!!");

    LogClose();
    return 0;
}