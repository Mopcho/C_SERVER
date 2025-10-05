#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <poll.h>

#include "LFS/server.h"
#include "LFS/pollfds_dynamic.h"
#include "LFS/connection.h"
#include "LFS/handler_codes.h"
#include "LFS/server_context.h"

const int listen_backlog = 5;

void lfs_accpet_connection(lfs_server_context * server_context)
{
    // accept
    struct sockaddr_storage incomingcon;
    socklen_t incomingcon_size = sizeof incomingcon;
    int newsockfd = accept(server_context->sockfd, (struct sockaddr*)&incomingcon, &incomingcon_size);
    if (newsockfd == -1)
    {
        perror("accept");
        exit(-1);
    }

    // create pollfd and add it to the server context
    struct pollfd pfd = { 0 };
    pfd.fd = newsockfd;
    pfd.events = POLLIN;
    lfs_pollfds_dynamic_add(server_context->pollfds_container, pfd);

    // create connection and add it to the server context
    lfs_connection conn = { 0 };
    conn.sockfd = newsockfd;
    lfs_connections_dynamic_add(server_context->connections_container, conn);
}

void lfs_receive_message(lfs_server_context * server_context, int sockfd)
{
    lfs_connection * connection = lfs_server_context_get_connection_by_fd(server_context, sockfd);

    ssize_t readbytes = recv(connection->sockfd, connection->readbuf, sizeof connection->readbuf - 1, 0);
    connection->readbuf[readbytes] = '\0';
    if (readbytes == -1 || readbytes == 0)
    {
        if (readbytes == -1) { perror("recv"); }
        close(sockfd);
        lfs_pollfds_dynamic_remove(server_context->pollfds_container, sockfd);
        return;
    }
    printf("PID message received: [%i]: %s \n", getpid(), connection->readbuf);
}

void process_pollin(lfs_server_context * server_context, struct pollfd pollevent)
{
    if (pollevent.fd == server_context->sockfd)
    {
        lfs_accpet_connection(server_context);
    } else
    {
        lfs_receive_message(server_context, pollevent.fd);
    }
}

void process_pollhup(lfs_server_context * server_context, struct pollfd pollevent)
{
    lfs_pollfds_dynamic_remove(server_context->pollfds_container, pollevent.fd);
}

void process_pollerr(lfs_server_context * server_context, struct pollfd pollevent)
{
    lfs_pollfds_dynamic_remove(server_context->pollfds_container, pollevent.fd);
}

void process_events(lfs_server_context * server_context)
{
    for (int i = 0; i < server_context->pollfds_container->size; i++)
    {
        struct pollfd pollevent = server_context->pollfds_container->pfds[i];

        if (pollevent.revents == 0)
        {
            continue;
        }

        // handle POLLIN
        if (pollevent.revents & POLLIN)
        {
            process_pollin(server_context, pollevent);
        }

        // handle hang up
        if (pollevent.revents & POLLHUP)
        {
            process_pollhup(server_context, pollevent);
            continue;
        }

        // handle POLLERR
        if (pollevent.revents & POLLERR)
        {
            process_pollerr(server_context, pollevent);
            perror("POLLERR");
        }
    }
}

int get_tcp_socket(const char* host, const char* port)
{
    struct addrinfo hints = { 0 };
    struct addrinfo *servinfo;
    int sockfd = -1;

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int status = getaddrinfo(host,port, &hints, &servinfo);
    if (status != 0) {
        fprintf(stderr, "gai error: %s\n", gai_strerror(status));
        return status;
    }
    struct addrinfo* p;
    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd == -1)
        {
            printf("Error acquiring socket; errno: %i", errno);
            continue;
        }

        int reuse_socket_opt = 1;
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse_socket_opt, sizeof reuse_socket_opt);

        int bindres = bind(sockfd, p->ai_addr, p->ai_addrlen);
        if (bindres == -1)
        {
            printf("Error binding socket; errno: %i \n", errno);
            continue;
        }

        break;
    }

    if (p == NULL)
    {
        return -1;
    }

    return sockfd;
}

int lfs_listen(const char* host, const char* port) {
    int sockfd = get_tcp_socket(host, port);
    int listenres = listen(sockfd, listen_backlog);
    if (listenres == -1)
    {
        printf("Error trying to listen; errno: %i \n", errno);
        return -1;
    }

    lfs_server_context* server_context = lfs_server_context_init();
    server_context->sockfd = sockfd;

    struct pollfd pollfd = { 0 };
    pollfd.fd = server_context->sockfd;
    pollfd.events = POLLIN;

    lfs_pollfds_dynamic_add(server_context->pollfds_container, pollfd);

    for (;;)
    {
        int pollerr = poll(server_context->pollfds_container->pfds, server_context->pollfds_container->size, -1);
        if (pollerr == -1)
        {
            perror("poll");
            exit(1);
        }

        process_events(server_context);
    }
}