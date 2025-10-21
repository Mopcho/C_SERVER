#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <unordered_map>

namespace lfs
{
    class Response
    {
    public:
        explicit Response();
        ~Response();

        void send(const std::string& buf);
        void set_status_code(int status);
        void set_header(const std::string& header, const std::string& value);
        std::string m_response_buffer;
    private:
        std::unordered_map<std::string, std::string> m_headers {};
        int m_statuscode;
        std::string m_statustext;
    };
}

#endif //RESPONSE_HPP
