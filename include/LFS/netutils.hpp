#ifndef NETUTILS_HPP
#define NETUTILS_HPP

#include <string>

namespace lfs
{
    int get_tcp_socket(const std::string& host, const std::string& port);
}

#endif //NETUTILS_HPP
