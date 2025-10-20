#include <utility>

#include "LFS/response.hpp"

namespace lfs
{
    Response::Response(int sockfd) : m_sockfd(sockfd)
    {
    }

    Response::~Response() = default;

    void Response::send(const std::string& response)
    {
        m_response_buffer = response;
    }
}
