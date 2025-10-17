#ifndef CONNECTION_H
#define CONNECTION_H

#include <string>
#include <unistd.h>
#include <unordered_map>
#include <vector>
#include <sys/types.h>

#include "request.hpp"

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

        void queueBufferWrite(const char* buf, size_t n);

        int receive();
        void parse_data_chunk(const char* buf);
        int get_sockfd() const;
    private:
        int m_sockfd;
        RequestMetadata metadata;
        std::string body;
        std::unordered_map<std::string, std::string> headers;
        std::vector<char> m_writebuf;
        Request request;
    };
}

#endif //CONNECTION_H
