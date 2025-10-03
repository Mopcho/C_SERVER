#ifndef SERVER_CONTEXT_H
#define SERVER_CONTEXT_H

#include "LFS/connection.h"
#include "LFS/pollfds_dynamic.h"

typedef struct
{
    lfs_connections_dynamic * connections;
    lfs_pollfds_dynamic * pollfds;
} lfs_server_context;

#endif //SERVER_CONTEXT_H
