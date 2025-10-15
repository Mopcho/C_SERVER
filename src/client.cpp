#include "LFS/client.hpp"

#include <netdb.h>
#include <array>
#include <stdexcept>
#include <fmt/format.h>

lfs::Client::Client(std::string host, std::string port) : m_host(std::move(host)), m_port(std::move(port)) {}

void lfs::Client::connect()
{
    addrinfo hints {};
    addrinfo * servinfo;

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(m_host.c_str(), m_port.c_str(), &hints, &servinfo) == -1)
    {
        throw std::runtime_error(fmt::format("could not get address info; errno: {:d}", errno));
    }

    int socketfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    if (socketfd == -1)
    {
        throw std::runtime_error(fmt::format("could not get socket; errno: {:d}", errno));
    }

    if (::connect(socketfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
    {
        throw std::runtime_error(fmt::format("could not connect socket; errno: {:d}", errno));
    }

    if (send(socketfd, "hello", 5, 0) == -1)
    {
        throw std::runtime_error(fmt::format("could not send message; errno: {:d}", errno));
    }

    std::array<char, 1024> recv_buf {};
    ssize_t recvres;
    while ((recvres = recv(socketfd, recv_buf.data(), recv_buf.size(), 0)) != -1)
    {
        printf("message received: %s \n", recv_buf.data());
        recv_buf.fill(0);
    }

    if (recvres == -1)
    {
        throw std::runtime_error(fmt::format("could not receive data from socket; errno: {:d}", errno));
    }
}
