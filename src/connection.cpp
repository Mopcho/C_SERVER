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
    LFS_LOG_DEBUG("%s \n", buf);
}

int lfs::Connection::get_sockfd()
{
    return m_sockfd;
}

ssize_t lfs::Connection::flushWriteBuffer()
{
    return send(m_sockfd, m_writebuf.data(), m_writebuf.size(), 0);
}
