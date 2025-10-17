#include "LFS/request.hpp"
#include <sstream>

#include "LFS/logger.hpp"
#include "LFS/stringutils.hpp"

namespace lfs
{
    Request::Request()
    = default;

    void Request::parse_bytes(const char* buf, size_t n)
    {
        std::string str_buf(buf, n);
        std::stringstream ss(str_buf);
        std::string line;

        // parse request line
        if (m_metadata.method.empty())
        {
            // Request line
            std::getline(ss, line);
            std::stringstream line_ss(line);
            line_ss >> m_metadata.method >> m_metadata.route >> m_metadata.version;
            m_parse_mode = LFS_RP_HEADERS;
        }

        // parse headers
        while (m_parse_mode == LFS_RP_HEADERS && std::getline(ss, line))
        {
            if (line.empty())
            {
                m_parse_mode = LFS_RP_CONTENT;
                break;
            }

            size_t colon_pos = line.find(':');
            if (colon_pos != std::string::npos)
            {
                std::string key =  line.substr(0, colon_pos);
                std::string value = line.substr(colon_pos + 1, std::string::npos);
                trim(key);
                trim(value);
                m_headers[key] = value;
            }
        }

        LFS_LOG_DEBUG("parsed request", NULL);

        // parse content
        std::streampos current_ss_pos = ss.tellg();
        std::string remaining = ss.str().substr(current_ss_pos, std::string::npos);
        m_content += remaining;
    }
}
