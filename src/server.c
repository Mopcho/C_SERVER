#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include "LFS/server.h"
#include <unistd.h>

#include <stdlib.h>

const int listen_backlog = 5;

size_t lfs_listen(const char* host, const char* port) {
    struct addrinfo hints;
    struct addrinfo *servinfo;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int status = getaddrinfo(host,port, &hints, &servinfo);
    if (status != 0) {
        fprintf(stderr, "gai error: %s\n", gai_strerror(status));
        return status;
    }

    for (struct addrinfo* p = servinfo; p != NULL; p = p->ai_next)
    {
        int sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd == -1)
        {
            printf("Error acquiring socket; errno: %i", errno);
            return sockfd;
        }

        int reuse_socket_opt = 1;
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse_socket_opt, sizeof reuse_socket_opt);

        int bindres = bind(sockfd, p->ai_addr, p->ai_addrlen);
        if (bindres == -1)
        {
            printf("Error binding socket; errno: %i \n", errno);
            return bindres;
        }

        int listenres = listen(sockfd, listen_backlog);
        if (listenres == -1)
        {
            printf("Error trying to listen; errno: %i \n", errno);
            return listenres;
        }

        while (1)
        {
            size_t acceptres = lfs_accept(sockfd);
            if (acceptres == -1)
            {
                printf("Error accept-ing; errno: %i \n", errno);
                return acceptres;
            }
        }

    }

    freeaddrinfo(servinfo);
    return 0;
}

size_t lfs_accept(int sockfd)
{
    struct sockaddr_storage incomingcon;
    socklen_t incomingcon_size = sizeof incomingcon;
    int newsockfd = accept(sockfd, (struct sockaddr*)&incomingcon, &incomingcon_size);
    if (newsockfd == -1)
    {
        perror("accept");
        return -1;
    }

    ssize_t readbytes;
    char buf[1025];

    while ((readbytes = recv(newsockfd, buf, sizeof buf - 1, 0)) > 0)
    {
        buf[readbytes] = '\0';
        printf("%s \n", buf);
    }

    if (readbytes == -1)
    {
        perror("recv");
        close(newsockfd);
        return -1;
    }

    close(newsockfd);
    return 0;
}