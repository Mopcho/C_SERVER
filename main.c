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
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int status = getaddrinfo(NULL, argv[1], &hints, &servinfo);
    if (status != 0) {
        fprintf(stderr, "gai error: %s\n", gai_strerror(status));
        exit(1);
    }

    for (struct addrinfo *p = servinfo; p != NULL; p = p->ai_next)
    {
        struct sockaddr_in *ipv4;
        struct sockaddr_in6 *ipv6;
        void* addr;
        char ipstr[INET6_ADDRSTRLEN];
        unsigned short port;

        if (p->ai_family == AF_INET6)
        {
            ipv6 = (struct sockaddr_in6*)p->ai_addr;
            addr = &ipv6->sin6_addr;
            port = ntohs(ipv6->sin6_port);
        } else if (p->ai_family == AF_INET)
        {
            ipv4 = (struct sockaddr_in*)p->ai_addr;
            addr = &ipv4->sin_addr;
            port = ntohs(ipv4->sin_port);
        }

        inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
        printf("Host & Port: %s:%hu \n", ipstr, port);

        int socketfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (socketfd == -1)
        {
            fprintf(stderr, "error while acquiring socket; errno: %i \n", errno);
            exit(1);
        }
        printf("socket file descriptor acquired: %i \n", socketfd);

        int bindres = bind(socketfd, p->ai_addr, p->ai_addrlen);
        if (bindres == -1)
        {
            fprintf(stderr, "error while binding socket: %i; errno: %i \n", socketfd, errno);
            exit(1);
        }
        printf("successfully bound socketfd: %i \n", socketfd);
    }

    freeaddrinfo(servinfo);
    return 0;
}
