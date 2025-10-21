#ifndef CONNECTION_H
#define CONNECTION_H

#include <memory>

#include "request.hpp"
#include "response.hpp"

#include <unistd.h>

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
        int receive() const;

        int m_sockfd;
        std::shared_ptr<Request> m_request;
        std::shared_ptr<Response> m_response;
    };
}

#endif //CONNECTION_H
