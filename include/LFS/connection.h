#ifndef CONNECTION_H
#define CONNECTION_H

typedef struct
{
    char readbuf[1024];
    char writebuf[1024];
    int sockfd;
} lfs_connection;

typedef struct
{
    lfs_connection * connections;
    int cap;
    int size;
} lfs_connection_dynamic;

void lfs_connection_dynamic_init(lfs_connection_dynamic * obj);

void lfs_connection_dynamic_add(lfs_connection_dynamic * obj, lfs_connection toadd);

void lfs_connection_dynamic_remove(lfs_connection_dynamic * obj, int sockfd_toremove);

void lfs_connection_dynamic_destruct(lfs_connection_dynamic * obj);

#endif //CONNECTION_H
