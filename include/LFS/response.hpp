#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>

namespace lfs
{
    class Response
    {
    public:
        explicit Response(int sockfd);
        ~Response();

        void send(const std::string& buf);
        std::string m_response_buffer;
        int m_sockfd;
    };
}

#endif //RESPONSE_HPP
