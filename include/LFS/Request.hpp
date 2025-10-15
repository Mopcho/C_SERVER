#ifndef REQUEST_HPP
#define REQUEST_HPP

namespace  lfs
{
    struct RequestMetadata
    {
        std::string route;
        std::string method;
        std::string version;
    };
}

#endif //REQUEST_HPP
