#include "Socket_click.h"

Socket_click::Socket_click(){
}

int Socket_click::set_socket(char* addr, int port){
    memset(&remote_addr,0,sizeof(remote_addr)); //数据初始化--清零
    remote_addr.sin_family=AF_INET; //设置为IP通信
    remote_addr.sin_addr.s_addr=inet_addr(addr);//服务器IP地址
    remote_addr.sin_port=htons(port); //服务器端口号
    /*创建客户端套接字--IPv4协议，面向连接通信，TCP协议*/
    if((client_sockfd=socket(PF_INET,SOCK_STREAM,0))<0){
        perror("socket");
        return -1;
    }
    /*将套接字绑定到服务器的网络地址上*/
    if(connect(client_sockfd,(struct sockaddr *)&remote_addr,sizeof(struct sockaddr))<0){
        perror("connect");
        return -1;
    }
    printf("connected to server\n");
    return 0;
}

int Socket_click::send_data(char* data){
    printf("string to send:");
    len=send(client_sockfd,data,strlen(data),0);
    return 1;
}

void Socket_click::rece_data(char* data){
    recv(client_sockfd, data, 3, 0);
}

Socket_click::~Socket_click(){
    close(client_sockfd);
}
