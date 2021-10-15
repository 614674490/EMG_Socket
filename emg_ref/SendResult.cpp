#include "SendResult.h"

using namespace std;

SendResult::SendResult(){
}

void SendResult::CreatServerSock(char* addr){
    //创建套接字
    printf("%s", addr);
    serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    //将套接字和IP、端口绑定
    memset(&serv_addr, 0, sizeof(serv_addr));  //每个字节都用0填充
    serv_addr.sin_family = AF_INET;  //使用IPv4地址
    serv_addr.sin_addr.s_addr = inet_addr(addr);  //具体的IP地址
    serv_addr.sin_port = htons(1234);  //端口
    bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    listen(serv_sock, 20);

    socklen_t clnt_addr_size = sizeof(clnt_addr);
    printf("=====");
    clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
    printf("-----");
    SendData(1);
   // pthread_t listenId;
   // int listenpro;
   // listenpro = pthread_create(&listenId, NULL, listenClick, NULL);
}

int SendResult::SendData(int label){
    //向客户端发送数据
    char *str;
    strcpy(str, label+"0");
    int writeResult = 0;
    writeResult = write(clnt_sock, str, sizeof(str));
    if (writeResult == sizeof(str))
        return 1;
    else
        return 0;
}

void *SendResult::listenClick(void *){
    listen(serv_sock, 20);
    socklen_t clnt_addr_size = sizeof(clnt_addr);
    clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
}

SendResult::~SendResult(){
    //关闭套接字
    close(clnt_sock);
    close(serv_sock);
}

