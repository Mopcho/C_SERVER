#ifndef POLLFDS_DYNAMIC_H
#define POLLFDS_DYNAMIC_H

#include <poll.h>

typedef struct
{
    struct pollfd * pfds;
    int size;
    int cap;
} lfs_pollfds_dynamic;

void lfs_pollfds_dynamic_init(lfs_pollfds_dynamic * obj);

void lfs_pollfds_dynamic_add(lfs_pollfds_dynamic * obj, struct pollfd toadd);

void lfs_pollfds_dynamic_remove(lfs_pollfds_dynamic * obj, int sockfd_toremove);

void lfs_pollfds_dynamic_destruct(lfs_pollfds_dynamic * obj);

#endif //POLLFDS_DYNAMIC_H
