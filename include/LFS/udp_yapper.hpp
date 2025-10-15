#ifndef UDP_YAPPER_HPP
#define UDP_YAPPER_HPP

namespace lfs::UDP
{
    int yap(const char* host, const char* port, const char * msgbuf, size_t msgbuf_size);
}

#endif //UDP_YAPPER_HPP
