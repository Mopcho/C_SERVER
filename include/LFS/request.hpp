#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <unordered_map>

#define LFS_RP_REQ_LINE 0
#define LFS_RP_HEADERS 1
#define LFS_RP_CONTENT 2

namespace  lfs
{
    class Request
    {
    public:
        Request();
        void parse_bytes(const char* buf, size_t n);
        bool has_received_all_content() const;

        std::unordered_map<std::string, std::string> m_headers {};
        std::string m_route;
        std::string m_method;
        std::string m_version;
        std::string m_content {};
        size_t m_received_content_bytes { 0 };
    private:
        int m_parse_mode { LFS_RP_REQ_LINE };
    };
}

#endif //REQUEST_HPP
