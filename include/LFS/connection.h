#ifndef CONNECTION_H
#define CONNECTION_H

#include <stddef.h>

typedef struct
{
    char readbuf[1024];
    char writebuf[1024];
    size_t readbytes;
    size_t writtenbytes;
    int sockfd;
} lfs_connection;

typedef struct
{
    lfs_connection * connections;
    int cap;
    int size;
} lfs_connections_dynamic;

lfs_connections_dynamic* lfs_connections_dynamic_init();

void lfs_connections_dynamic_add(lfs_connections_dynamic * obj, lfs_connection toadd);

void lfs_connections_dynamic_remove(lfs_connections_dynamic * obj, int sockfd_toremove);

void lfs_connections_dynamic_destruct(lfs_connections_dynamic * obj);

#endif //CONNECTION_H
