#ifndef SENDRESULT_H_INCLUDED
#define SENDRESULT_H_INCLUDED
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;

class SendResult{
private:
    int serv_sock;
    int clnt_sock;
    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;

public:
    SendResult();
    void CreatServerSock(char* addr);
    int SendData(int label);
	void *listenClick(void *);
    ~SendResult();
};
#endif
