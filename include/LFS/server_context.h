#ifndef SERVER_CONTEXT_H
#define SERVER_CONTEXT_H

#include "LFS/connection.h"
#include "LFS/pollfds_dynamic.h"

typedef struct
{
    lfs_connections_dynamic * connections_container;
    lfs_pollfds_dynamic * pollfds_container;
    int sockfd;
} lfs_server_context;

lfs_server_context* lfs_server_context_init();

lfs_connection * lfs_server_context_get_connection_by_fd(lfs_server_context* server_context, int fd);

#endif //SERVER_CONTEXT_H
