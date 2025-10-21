#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <vector>
#include <memory>
#include <sys/poll.h>
#include <unordered_map>
#include "connection.h"
#include <functional>

#define LFS_METADATA_SEPARATOR_TOKEN "\n\n"

namespace lfs
{
    extern const int listen_backlog;

    using HandlerFn = std::function<void(Request* req, Response* res)>;

    class Server
    {
    public:
        Server(std::string host, std::string port);
        int listen();
        void close() const;
        ~Server();
        void handle(const std::string & route, HandlerFn handler);

        std::string m_port;
        std::string m_host;
    private:
        int m_serversockfd;
        std::vector<pollfd> m_pollfds {};
        std::unordered_map<int, std::unique_ptr<Connection>> m_connections {};
        std::unordered_map<std::string, HandlerFn> m_handlers {};

        void process_pollevents();
        void process_pollout(pollfd& pollevent);
        void process_pollerr(const pollfd& pollevent);
        void process_pollhup(const pollfd& pollevent);
        void process_pollin(pollfd& pollevent);

        void accpet_connection();
        void remove_connection(int socketfd);

        void pollfd_add(int sockfd, int eventflags);
        void pollfd_remove(int sockfd);
    };
}

#endif //SERVER_H
