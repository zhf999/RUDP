#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

void err(const char *);
void InitAddr(sockaddr_in *addr, const char* ip, const char *port);

int main(int argc,char *argv[]) {
    if(argv[1][0]=='c')
    {
        int send_sock = socket(PF_INET,SOCK_DGRAM,0);
        if(-1==send_sock)
            err("Init socket error!");
        sockaddr_in serv_addr,client_addr;
        InitAddr(&serv_addr, argv[3], argv[4]);
        InitAddr(&client_addr, argv[3], argv[2]);
//        if(bind(send_sock,(const sockaddr*)&client_addr,sizeof(sockaddr_in))==-1)
//            err("Bind error!");
        if(connect(send_sock,(sockaddr*)&serv_addr,sizeof(sockaddr))==-1)
            err("Connect error!");
        char buf[1024]= "Hello World!";
        send(send_sock, buf, strlen(buf), 0);
    }
    else if(argv[1][0]=='s')
    {
        int recv_sock = socket(PF_INET, SOCK_DGRAM, 0);
        if(-1 == recv_sock)
            err("Init socket error!");
        sockaddr_in serv_addr,client_addr;
        memset(&client_addr,0,sizeof(client_addr));
        InitAddr(&serv_addr, nullptr, argv[2]);
        if(bind(recv_sock, (const sockaddr*)&serv_addr, sizeof(sockaddr_in)) == -1)
            err("Bind error!");
        char buf[1024];
        socklen_t addr_len=sizeof(client_addr);
        int n = recvfrom(recv_sock,(void*)buf,1024,0,(sockaddr*)&client_addr,&addr_len);
        buf[n]='\n';
        char ipv4[32];
        inet_ntop(AF_INET,(void*)&client_addr.sin_addr,ipv4,sizeof(ipv4));
        printf("Received from %s %d %d bytes:\n", ipv4, ntohs(client_addr.sin_port),n);
        printf("%s",buf);
    }


    return 0;
}

void err(const char *s)
{
    printf("%s\n",s);
    exit(0);
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
