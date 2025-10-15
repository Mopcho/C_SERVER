#ifndef CLIENT_HPP
#define CLIENT_HPP
#include <string>

namespace lfs
{
    class Client
    {
    public:
        Client(std::string host, std::string port);
        void connect();
    private:
        std::string m_host;
        std::string m_port;
    };
}

#endif //CLIENT_HPP
