#ifndef SOCKET_CLICK_H_INCLUDED
#define SOCKET_CLICK_H_INCLUDED

#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>


class Socket_click{
public:
    int client_sockfd;
    int len;
    int BUFSIZE = 1024;
    char databuf[1024];
    struct sockaddr_in remote_addr; //服务器端网络地址结构体

    Socket_click();
    int set_socket(char* addr, int port);
    int send_data(char* data);
    void rece_data(char* data);
    ~Socket_click();
};
#endif

