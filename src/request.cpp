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

        size_t line_start = 0;
        size_t line_end = str_buf.find('\n', line_start);

        // parse request line
        if (m_method.empty())
        {
            // Request line
            size_t method_end = str_buf.find(' ', line_start);
            size_t route_end = str_buf.find(' ', method_end + 1);

            m_method = str_buf.substr(line_start, method_end);
            m_route = str_buf.substr(method_end + 1, route_end - method_end - 1);
            m_version = str_buf.substr(route_end + 1, line_end - route_end - 1);
            remove_tailing_carriage_return(m_version);

            line_start = line_end + 1;
            line_end = str_buf.find('\n', line_start);

            m_parse_mode = LFS_RP_HEADERS;
        }

        // parse headers
        while (m_parse_mode == LFS_RP_HEADERS && line_end != std::string::npos)
        {
            std::string line_str { str_buf.substr(line_start, line_end - line_start) };
            remove_tailing_carriage_return(line_str);

            if (line_str.empty())
            {
                m_parse_mode = LFS_RP_CONTENT;
                break;
            }

            size_t colon_pos = str_buf.find(':', line_start);
            if (colon_pos != std::string::npos)
            {
                std::string key =  str_buf.substr(line_start, colon_pos - line_start);
                std::string value = str_buf.substr(colon_pos + 1, line_end - colon_pos);
                trim(key);
                trim(value);
                m_headers[key] = value;
            } else
            {
                LFS_LOG_WARNING("colon position not found during parsing of header line", NULL);
            }

            m_received_content_bytes += line_end - line_start;
            line_start = line_end + 1;
            line_end = str_buf.find('\n', line_start);
        }

        LFS_LOG_DEBUG("parsed request", NULL);

        // parse content
        m_content.append(str_buf.substr(line_end, std::string::npos));
    }

    bool Request::has_received_all_content() const
    {
        try
        {
            int content_length = stoi(m_headers.at("Content-Length"));
            return m_received_content_bytes >= content_length;
        } catch (std::out_of_range &)
        {
            LFS_LOG_ERROR("Content-Length header not present in request", NULL);
            return true;
        }
    }
}
