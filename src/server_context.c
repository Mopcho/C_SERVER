#include "LFS/server_context.h"

#include <stdlib.h>
#include <string.h>

lfs_server_context* lfs_server_context_init()
{
    lfs_server_context* server_context = malloc(sizeof(lfs_server_context));
    memset(server_context, 0, sizeof(lfs_server_context));
    server_context->connections_container = lfs_connections_dynamic_init();
    server_context->pollfds_container = lfs_pollfds_dynamic_init();
    return server_context;
}

lfs_connection * lfs_server_context_get_connection_by_fd(lfs_server_context* server_context, int fd)
{
    for (int i = 0; i < server_context->connections_container->size; i++)
    {
        if (server_context->connections_container->connections[i].sockfd == fd)
        {
            return &server_context->connections_container->connections[i];
        }
    }
}