#ifndef SOCKET_H_INCLUDED
#define SOCKET_H_INCLUDED

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define SENDBUFFLEN 4
#define IP (char *)("192.168.43.195")
#define PORT 225
class Socket
{
public:
    int client_sockfd;
    int len;
    struct sockaddr_in remote_addr;                 //服务器端网络地址结构体
    char sendBuf[SENDBUFFLEN] = {0xAA, 0, 0, 0xff}; //帧头 数据 帧尾

    Socket(char *addr, int port);
    bool connectServer();
    int send_data(char *data);
    void rece_data(char *data);
    ~Socket();
};
extern bool con_flag;
#endif
