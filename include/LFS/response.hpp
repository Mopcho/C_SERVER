#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>

namespace lfs
{
    class Response
    {
    public:
        explicit Response();
        ~Response();

        void send(const std::string& buf);
        std::string m_response_buffer;
    };
}

#endif //RESPONSE_HPP
