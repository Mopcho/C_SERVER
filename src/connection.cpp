#include "LFS/connection.h"

#include <array>
#include <sys/socket.h>

lfs::Connection::Connection(int socketfd) :  m_sockfd(socketfd) {}

int lfs::Connection::receive()
{
    std::array<char, 4096> buf {};
    ssize_t bytes_read = recv(m_sockfd, buf.data(), buf.size(), 0);
    if (bytes_read == -1) {
        return -1;
    }
    request.parse_bytes(buf.data(), bytes_read);
    return 0;
}

int lfs::Connection::get_sockfd() const
{
    return m_sockfd;
}

ssize_t lfs::Connection::flushWriteBuffer()
{
    auto sent = send(m_sockfd, m_writebuf.data(), m_writebuf.size(), 0);
    m_writebuf.clear();
    return sent;
}

void lfs::Connection::queueBufferWrite(const char* buf, size_t n) {
    m_writebuf.reserve(m_writebuf.capacity() + n);
    m_writebuf.insert(m_writebuf.end(), buf, buf + n);
}
