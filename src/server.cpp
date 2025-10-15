#include "LFS/server.hpp"
#include "LFS/logger.hpp"

#include <cerrno>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <poll.h>
#include <stdexcept>
#include <utility>
#include <fmt/format.h>

const int lfs::listen_backlog = 5;

void lfs::Server::accpet_connection()
{
    // accept
    sockaddr_storage incomingcon {};
    socklen_t incomingcon_size = sizeof incomingcon;
    int newsockfd = accept(m_sockfd, reinterpret_cast<struct sockaddr*>(&incomingcon), &incomingcon_size);
    if (newsockfd == -1)
    {
        perror("accept");
        exit(-1);
    }

    // create pollfd and add it to the server context
    m_pollfds.emplace_back(newsockfd, POLLIN);

    // create connection and add it to the server context
    m_connections[newsockfd] = std::make_unique<Connection>(newsockfd);
}

void lfs::Server::process_pollin(pollfd& pollevent)
{
    if (pollevent.fd == m_sockfd)
    {
        accpet_connection();
    }
    else
    {
        auto connection = m_connections.find(pollevent.fd);
        if (connection == m_connections.end())
        {
            LFS_LOG_WARNING("cant find connection for %d while processing pollin event \n", pollevent.fd);
            return;
        }

        if (connection->second->receive() == -1)
        {
            remove_connection(connection->second->get_sockfd());
            return;
        }

        const char* msg = "lalala\n";
        connection->second->queueBufferWrite(msg, std::strlen(msg));

        pollevent.events |= POLLOUT;
    }
}

void lfs::Server::remove_connection(int socketfd)
{
    m_connections.erase(socketfd);
    for (int i = 0; i < m_pollfds.size(); i++)
    {
        if (m_pollfds[i].fd == socketfd)
        {
            m_pollfds.erase(m_pollfds.begin() + i);
        }
    }
}

void lfs::Server::process_pollhup(const pollfd& pollevent)
{
    remove_connection(pollevent.fd);
}

void lfs::Server::process_pollerr(const pollfd& pollevent)
{
    remove_connection(pollevent.fd);
}

void lfs::Server::process_pollout(pollfd& pollevent)
{
    auto conn = m_connections.find(pollevent.fd);
    if (conn == m_connections.end())
    {
        LFS_LOG_WARNING("pollout event on fd: %d but no connection found for this file descriptor \n", pollevent.fd);
        return;
    }

    if (conn->second->flushWriteBuffer())
    {
        pollevent.events &= ~POLLOUT;
    }
}

void lfs::Server::process_pollevents()
{
    for (auto &pollevent : m_pollfds)
    {
        LFS_LOG_DEBUG("processing pollfd socket: %i; revents mask: %i \n", pollevent.fd, pollevent.revents);

        if (pollevent.revents == 0) { continue; }

        if (pollevent.revents & POLLIN)
        {
            process_pollin(pollevent);
        }

        if (pollevent.revents & POLLOUT)
        {
            process_pollout(pollevent);
        }

        if (pollevent.revents & POLLHUP)
        {
            process_pollhup(pollevent);
            continue;
        }

        if (pollevent.revents & POLLERR)
        {
            process_pollerr(pollevent);
            perror("POLLERR");
        }
    }
}

int get_tcp_socket(const std::string& host, const std::string& port)
{
    addrinfo hints {};
    addrinfo* servinfo;
    int sockfd = -1;

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (int status = getaddrinfo(host.c_str(), port.c_str(), &hints, &servinfo); status != 0)
    {
        fprintf(stderr, "gai error: %s\n", gai_strerror(status));
        return status;
    }
    addrinfo* p;
    for (p = servinfo; p != nullptr; p = p->ai_next)
    {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd == -1)
        {
            printf("Error acquiring socket; errno: %i", errno);
            continue;
        }

        int reuse_socket_opt = 1;
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse_socket_opt, sizeof reuse_socket_opt);

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            printf("Error binding socket; errno: %i \n", errno);
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

lfs::Server::Server(std::string host, std::string port) : m_host(std::move(host)), m_port(std::move(port))
{
    m_pollfds.reserve(20);
    m_connections.reserve(20);
    m_sockfd = get_tcp_socket(m_host, m_port);
}

int lfs::Server::listen()
{
    if (::listen(m_sockfd, listen_backlog) == -1)
    {
        throw std::runtime_error(fmt::format("error trying to listen on host: {:s} and port: {:s}; errno: {:d}", m_host, m_port, errno));
    }

    m_pollfds.emplace_back(m_sockfd, POLLIN);

    for (;;)
    {
        if (poll(m_pollfds.data(), m_pollfds.size(), -1) == -1)
        {
            throw std::runtime_error(fmt::format("error polling for events; errno: {:d}", errno));
        }

        process_pollevents();
    }
}