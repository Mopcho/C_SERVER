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

#include "LFS/netutils.hpp"

const int lfs::listen_backlog = 5;

void lfs::Server::accpet_connection()
{
    // accept
    sockaddr_storage incomingcon {};
    socklen_t incomingcon_size = sizeof incomingcon;
    int newsockfd = accept(m_sockfd, reinterpret_cast<struct sockaddr*>(&incomingcon), &incomingcon_size);
    if (newsockfd == -1)
    {
        throw std::runtime_error("error trying to accept connection");
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

        size_t bytes_read = connection->second->receive();
        if (bytes_read == -1 || bytes_read == 0)
        {
            remove_connection(connection->second->get_sockfd());
            return;
        }

        Request* request = connection->second->m_request.get();
        Response* response = connection->second->m_response.get();
        if (request->has_received_all_content())
        {
            try
            {
                HandlerFn handler = m_handlers.at(request->m_metadata.route);
                handler(request, response);
                pollevent.events |= POLLOUT;
            } catch (std::out_of_range & err)
            {
                LFS_LOG_DEBUG("No handler found\n", NULL);
            }
        }
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

    Response* response = conn->second->m_response.get();
    if (!response->m_response_buffer.empty())
    {
        ::send(response->m_sockfd, response->m_response_buffer.data(), response->m_response_buffer.size(), 0);
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

void lfs::Server::close() const
{
    shutdown(m_sockfd, SHUT_WR);
    std::array<char, 1024> buf {};
    for (;;)
    {
        int status = read(m_sockfd, buf.data(), buf.size());
        if (status == 0)
        {
            break;
        }

        if (status == -1)
        {
            LFS_LOG_WARNING("error reading from scoket", NULL);
            break;
        }
    }

    ::close(m_sockfd);
}

lfs::Server::~Server()
{
    close();
}

void lfs::Server::handle(const std::string & route, HandlerFn handler)
{
    m_handlers[route] = std::move(handler);
}
