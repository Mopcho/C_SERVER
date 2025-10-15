#ifndef CONNECTION_H
#define CONNECTION_H

#include <string>
#include <unistd.h>
#include <unordered_map>
#include <vector>
#include <sys/types.h>

#include "Request.hpp"

namespace lfs
{
    class Connection
    {
    public:
        explicit Connection(int socketfd);

        ~Connection()
        {
            close(m_sockfd);
        }

        ssize_t flushWriteBuffer();

        template <size_t N>
        void queueBufferWrite(std::array<char, N> buf)
        {
            m_writebuf.reserve(m_writebuf.capacity() + N);
            m_writebuf.insert(m_writebuf.end(), buf.begin(), buf.end()); // TODO: This will do a copy. Find smth more performant
        }

        int receive();
        void parse_data_chunk(const char* buf);
        int get_sockfd();
    private:
        int m_sockfd;
        RequestMetadata metadata;
        std::string body;
        std::unordered_map<std::string, std::string> headers;
        std::vector<char> m_writebuf;
    };
}

#endif //CONNECTION_H
