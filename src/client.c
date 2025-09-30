#include "LFS/client.h"
#include <netdb.h>
#include <stdio.h>
#include <string.h>

int lfs_connect(const char* host, const char* port)
{
    struct addrinfo hints = { 0 };
    struct addrinfo * servinfo;

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    getaddrinfo(host, port, &hints, &servinfo);

    int socketfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    if (socketfd == -1)
    {
        perror("socket");
        return -1;
    }

    int connres = connect(socketfd, servinfo->ai_addr, servinfo->ai_addrlen);
    if (connres == -1)
    {
        perror("connect");
        return -1;
    }

    int sendres = send(socketfd, "hello", 5, 0);
    if (sendres == -1)
    {
        perror("send");
        return -1;
    }

    char recv_buf[1024];
    int recvres;
    while ((recvres = recv(socketfd, recv_buf, 1024, 0)) != -1)
    {
        printf("message received: %s \n", recv_buf);
        memset(recv_buf, 0, 1024);
    }

    if (recvres == -1)
    {
        perror("recv");
        return -1;
    }

    return 0;
}
