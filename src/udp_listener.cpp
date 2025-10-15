#include <cstring>
#include <netdb.h>
#include <cstdio>

namespace lfs::UDP
{
    int listen(const char* host, const char* port)
    {
        addrinfo hints {};
        addrinfo* servinfo;

        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_DGRAM;
        hints.ai_flags = AI_PASSIVE;

        int getaddrres = getaddrinfo(host, port, &hints, &servinfo);
        if (getaddrres == -1)
        {
            perror("getaddrres");
            return -1;
        }

        int socketfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
        if (socketfd == -1)
        {
            perror("socket");
            return -1;
        }

        int bindres = bind(socketfd, servinfo->ai_addr, servinfo->ai_addrlen);
        if (bindres == -1)
        {
            perror("bind");
            return -1;
        }

        char buf[1024];
        memset(buf, 0, sizeof buf);

        ssize_t recvfromres;
        while ((recvfromres = recvfrom(socketfd, buf, sizeof buf, 0, nullptr, nullptr)) != -1)
        {
            printf("%s \n", buf);
            memset(buf, 0, sizeof buf);
        }
        if (recvfromres == -1)
        {
            perror("recvfromres");
            return -1;
        }

        return 0;
    }
}
