#ifndef POLLFDS_DYNAMIC_H
#define POLLFDS_DYNAMIC_H

#include <poll.h>

typedef struct
{
    struct pollfd * pfds;
    int size;
    int cap;
} pollfds_dynamic;

void pollfds_dynamic_init(pollfds_dynamic * obj);

void pollfds_dynamic_add(pollfds_dynamic * obj, struct pollfd toadd);

void pollfds_dynamic_remove(pollfds_dynamic * obj, int sockfd_toremove);

void pollfds_dynamic_destruct(pollfds_dynamic * obj);

#endif //POLLFDS_DYNAMIC_H
