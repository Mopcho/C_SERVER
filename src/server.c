#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <poll.h>

#include "LFS/server.h"
#include "LFS/pollfds_dynamic.h"

const int listen_backlog = 5;

void handle_accept(pollfds_dynamic * pfds_dyn, int hsockfd)
{
    struct sockaddr_storage incomingcon;
    socklen_t incomingcon_size = sizeof incomingcon;
    int newsockfd = accept(hsockfd, (struct sockaddr*)&incomingcon, &incomingcon_size);
    if (newsockfd == -1)
    {
        perror("accept");
        exit(-1);
    }

    struct pollfd pfd = { 0 };
    pfd.fd = newsockfd;
    pfd.events = POLLIN;

    pollfds_dynamic_add(pfds_dyn, pfd);
}

void handle_recv(pollfds_dynamic * pfds_dyn, int recvsockfd)
{
    char buf[1025];

    ssize_t readbytes = recv(recvsockfd, buf, sizeof buf - 1, 0);

    if (readbytes == -1)
    {
        perror("recv");
        close(recvsockfd);
        pollfds_dynamic_remove(pfds_dyn, recvsockfd);
        exit(-1);
    }

    buf[readbytes] = '\0';
    printf("PID message received: [%i]: %s \n", getpid(), buf);

    send(recvsockfd, "hi", 3, 0); // TODO: I think this also should be based on event because we might not be ready to send still and block the thread
}

void process_poll_fds(pollfds_dynamic * pfds_dyn, int hsockfd)
{
    for (int i = 0; i < pfds_dyn->size; i++)
    {
        struct pollfd pollevent = pfds_dyn->pfds[i];

        if (pollevent.revents == 0)
        {
            continue;
        }

        // handle POLLIN
        if (pollevent.revents & POLLIN)
        {
            if (pollevent.fd == hsockfd)
            {
                handle_accept(pfds_dyn, hsockfd);
            } else
            {
                handle_recv(pfds_dyn, pollevent.fd);
            }
        }

        // handle hang up
        if (pollevent.revents & POLLHUP)
        {
            perror("POLLHUP");
            pollfds_dynamic_remove(pfds_dyn, pollevent.fd);
            continue;
        }

        // handle POLLERR
        if (pollevent.revents & POLLERR)
        {
            perror("POLLERR");
            exit(-1);
        }
    }
}

size_t lfs_listen(const char* host, const char* port) {
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    struct addrinfo *servinfo;
    int sockfd;

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

    int listenres = listen(sockfd, listen_backlog);
    if (listenres == -1)
    {
        printf("Error trying to listen; errno: %i \n", errno);
        return -1;
    }

    // Listen for new connection on the original socket
    pollfds_dynamic pollfds_dyn;
    pollfds_dynamic_init(&pollfds_dyn);

    struct pollfd pollfd = { 0 };
    pollfd.fd = sockfd;
    pollfd.events = POLLIN;

    pollfds_dynamic_add(&pollfds_dyn, pollfd);

    for (;;)
    {
        int pollerr = poll(pollfds_dyn.pfds, pollfds_dyn.size, -1);
        if (pollerr == -1)
        {
            perror("poll");
            exit(1);
        }

        process_poll_fds(&pollfds_dyn, sockfd);
    }
}