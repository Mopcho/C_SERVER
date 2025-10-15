#include "LFS/connection.h"

#include <array>
#include <sys/socket.h>

#include "LFS/logger.hpp"

lfs::Connection::Connection(int socketfd) :  m_sockfd(socketfd) {}

int lfs::Connection::receive()
{
    std::array<char, 1024> buf {};
    if (recv(m_sockfd, buf.data(), buf.size(), 0) == -1) {
        return -1;
    }
    parse_data_chunk(buf.data());
    return 0;

}

void lfs::Connection::parse_data_chunk(const char* buf)
{
    LFS_LOG_DEBUG("Parsing data chunk: %s \n", buf);
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
