#include "LFS/response.hpp"

namespace lfs
{
    Response::Response() = default;
    Response::~Response() = default;

    void Response::send(const std::string& buf)
    {
        m_response_buffer = buf;
    }
}
