#include "LFS/connection.h"

#include <array>
#include <sys/socket.h>

lfs::Connection::Connection(int socketfd) : m_sockfd(socketfd), m_request(std::make_shared<Request>()), m_response(std::make_shared<Response>()) {}

// TODO: What if 4096 bytes in and we still reading headers
int lfs::Connection::receive() const
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
