#ifndef SERVER_H
#define SERVER_H

extern const int listen_backlog;

size_t lfs_listen(const char* host, const char* port);

size_t lfs_accept(int sockfd);

#endif //SERVER_H
