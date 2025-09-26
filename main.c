#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Wrong count of arguments passed. Expected 1.");
        exit(1);
    }

    struct addrinfo hints;
    struct addrinfo *servinfo;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int status = getaddrinfo(NULL,argv[1], &hints, &servinfo);
    if (status != 0) {
        fprintf(stderr, "gai error: %s\n", gai_strerror(status));
        exit(1);
    }

    for (struct addrinfo* p = servinfo; p != NULL; p = p->ai_next)
    {
        int listen_backlog = 5;

        int sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd == -1)
        {
            printf("Error acquiring socket");
            exit(1);
        }

        int reuse_socket_opt = 1;
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse_socket_opt, sizeof reuse_socket_opt);

        int bindres = bind(sockfd, p->ai_addr, p->ai_addrlen);
        if (bindres == -1)
        {
            printf("Error binding socket");
            exit(1);
        }

        int listenres = listen(sockfd, listen_backlog);
        if (listenres == -1)
        {
            printf("Error trying to listen");
            exit(1);
        }

        struct sockaddr_storage incomingcon;
        socklen_t incomingcon_size = sizeof incomingcon;
        int newsockfd = accept(sockfd, (struct sockaddr*)&incomingcon, &incomingcon_size);
        if (newsockfd == -1)
        {
            printf("Error accept-ing; errno: %i \n", errno);
            exit(1);
        }

        char buf[1024];
        size_t readbytes;
        while ((readbytes = recv(newsockfd, buf, sizeof buf, 0)))
        {
            printf("%s \n", buf);
            memset(buf, 0, sizeof buf);
        }
        if (readbytes == -1)
        {
            printf("Error receiving bytes; errno: %i \n", errno);
            exit(1);
        }
    }

    freeaddrinfo(servinfo);
    return 0;
}
