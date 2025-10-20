#include "LFS/connection.h"

#include <array>
#include <sys/socket.h>

lfs::Connection::Connection(int socketfd) : m_response(std::make_shared<Response>(socketfd)), m_request(std::make_shared<Request>()), m_sockfd(socketfd)
{
}

// TODO: What if 4096 bytes in and we still reading headers
int lfs::Connection::receive()
{
    std::array<char, 4096> buf {};
    ssize_t bytes_read = recv(m_sockfd, buf.data(), buf.size(), 0);
    if (bytes_read == -1) {
        return -1;
    }
    if (bytes_read == 0)
    {
        return 0;
    }
    m_request->parse_bytes(buf.data(), bytes_read);
    return 1;
}

int lfs::Connection::get_sockfd() const
{
    return m_sockfd;
}
