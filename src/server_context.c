#include "LFS/server_context.h"

#include <stdlib.h>
#include <string.h>

lfs_server_context* lfs_server_context_init()
{
    lfs_server_context* server_context = malloc(sizeof(lfs_server_context));
    memset(server_context, 0, sizeof(lfs_server_context));
    server_context->connections = lfs_connections_dynamic_init();
    server_context->pollfds = lfs_pollfds_dynamic_init();
    return server_context;
}
