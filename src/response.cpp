#include "LFS/response.hpp"
#include <fmt/core.h>

namespace lfs
{
    Response::Response() : m_statuscode(200)
    {
        m_headers["Content-Type"] = "text/plain; charset=UTF-8";
        m_headers["Server"] = "HMTP/0.1";
        m_headers["Connection"] = "Close";
    };

    Response::~Response() = default;

    void Response::send(const std::string& buf)
    {
        m_response_buffer = fmt::format("HTTP/1.1 {} {}\r\n", m_statuscode, m_statustext);

        for (auto& [key, value] : m_headers)
        {
            m_response_buffer.append(fmt::format("{}:{}\r\n", key, value));
        }

        m_response_buffer.append("\r\n");
        m_response_buffer.append(buf);
    }

    void Response::set_status_code(int status)
    {
        m_statuscode = status;
        switch (status)
        {
        case 200: m_statustext = "OK"; break;
        case 201: m_statustext = "Created"; break;
        case 202: m_statustext = "Accepted"; break;
        case 204: m_statustext = "No Content"; break;
        case 301: m_statustext = "Moved Permanently"; break;
        case 400: m_statustext = "Bad Request"; break;
        case 401: m_statustext = "Unauthorized"; break;
        case 403: m_statustext = "Forbidden"; break;
        case 404: m_statustext = "Not Found"; break;
        case 405: m_statustext = "Method Not Allowed"; break;
        case 411: m_statustext = "Length Required"; break;
        case 415: m_statustext = "Unsupported Media Type"; break;
        case 431: m_statustext = "Request Header Fields Too Large"; break;
        case 500: m_statustext = "Internal Server Error"; break;
        case 501: m_statustext = "Not Implemented"; break;
        case 502: m_statustext = "Bad Gateway"; break;
        case 505: m_statustext = "HTTP Version Not Supported"; break;
        }
    }

    void Response::set_header(const std::string& header, const std::string& value)
    {
        m_headers[header] = value;
    }
}
