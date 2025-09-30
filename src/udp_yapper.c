#include <netdb.h>
#include <stdio.h>
#include <string.h>

int lfs_yap(const char* host, const char* port, const char * msgbuf, size_t msgbuf_size)
{
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);

    struct addrinfo * servinfo;

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    if (getaddrinfo(host, port, &hints, &servinfo) == -1)
    {
        perror("getaddrinfo");
        return -1;
    }

    int socketfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    if (socketfd == -1)
    {
        perror("socket");
        return -1;
    }

    int sentbytes = sendto(socketfd, msgbuf, msgbuf_size, 0, servinfo->ai_addr, servinfo->ai_addrlen);
    if (sentbytes == -1)
    {
        perror("sendto");
        return -1;
    }

    freeaddrinfo(servinfo);

    return 0;
}
