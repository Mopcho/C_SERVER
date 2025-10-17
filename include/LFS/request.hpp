#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <unordered_map>

#define LFS_RP_REQ_LINE 0
#define LFS_RP_HEADERS 1
#define LFS_RP_CONTENT 2

namespace  lfs
{
    struct RequestMetadata
    {
        std::string route;
        std::string method;
        std::string version;
    };

    class Request
    {
    public:
        Request();

        void parse_bytes(const char* buf, size_t n);
    private:
        std::unordered_map<std::string, std::string> m_headers {};
        RequestMetadata m_metadata {};
        std::string m_content {};
        int m_parse_mode { LFS_RP_REQ_LINE };
    };
}

#endif //REQUEST_HPP
