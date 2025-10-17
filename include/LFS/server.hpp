#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <vector>
#include <memory>
#include <sys/poll.h>
#include <unordered_map>
#include "connection.h"

#define LFS_METADATA_SEPARATOR_TOKEN "\n\n"

namespace lfs
{
    extern const int listen_backlog;

    class Server
    {
    public:
        Server(std::string host, std::string port);
        int listen();
        void close() const;
        ~Server();

    private:
        std::string m_host;
        std::string m_port;
        int m_sockfd;
        std::vector<pollfd> m_pollfds {};
        std::unordered_map<int, std::unique_ptr<Connection>> m_connections {};

        void process_pollevents();
        void process_pollout(pollfd& pollevent);
        void process_pollerr(const pollfd& pollevent);
        void process_pollhup(const pollfd& pollevent);
        void process_pollin(pollfd& pollevent);
        void accpet_connection();
        void remove_connection(int socketfd);
    };
}

#endif //SERVER_H
