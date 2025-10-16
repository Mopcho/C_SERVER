#include <LFS/netutils.hpp>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdexcept>
#include <fmt/core.h>

#include "LFS/logger.hpp"

namespace lfs
{
    int get_tcp_socket(const std::string& host, const std::string& port)
    {
        addrinfo hints{};
        addrinfo* servinfo;
        int sockfd = -1;

        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;

        if (int status = getaddrinfo(host.c_str(), port.c_str(), &hints, &servinfo); status != 0)
        {
            throw std::runtime_error(fmt::format("getaddrinfo gai error: %s\n", gai_strerror(status)));
        }
        addrinfo* p;
        for (p = servinfo; p != nullptr; p = p->ai_next)
        {
            sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
            if (sockfd == -1)
            {
                LFS_LOG_DEBUG("Error acquiring socket; errno: %i", errno);
                continue;
            }

            int reuse_socket_opt = 1;
            setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse_socket_opt, sizeof reuse_socket_opt);

            if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
            {
                LFS_LOG_DEBUG("Error acquiring socket; errno: %i", errno);
                continue;
            }

            break;
        }

        if (p == nullptr)
        {
            throw std::runtime_error("could not get a socket to bind to");
        }

        return sockfd;
    }
}
