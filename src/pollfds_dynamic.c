#include "LFS/pollfds_dynamic.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void lfs_pollfds_dynamic_add(lfs_pollfds_dynamic * obj, struct pollfd toadd)
{
    if (obj->size == obj->cap)
    {
        // grow
        int new_cap = 4;
        if (obj->cap != 0)
        {
            new_cap = obj->cap * 2;
        }

        // allocate new bigger memory block
        struct pollfd * new_memory_block = malloc(new_cap * sizeof(struct pollfd));
        if (new_memory_block == NULL)
        {
            perror("malloc");
            exit(-1);
        }

        // move from old to new memory block
        memcpy(new_memory_block, obj->pfds, obj->size * sizeof(struct pollfd));
        free(obj->pfds);
        obj->pfds = new_memory_block;
        obj->cap = new_cap;
    }

    obj->pfds[obj->size] = toadd;
    obj->size++;
}

void lfs_pollfds_dynamic_remove(lfs_pollfds_dynamic * obj, int sockfd_toremove)
{
    for (int i = 0; i < obj->size; i++)
    {
        if (obj->pfds[i].fd == sockfd_toremove)
        {
            memset(&obj->pfds[i], 0, sizeof(struct pollfd));
        }
    }
}

void lfs_pollfds_dynamic_init(lfs_pollfds_dynamic * obj)
{
    memset(obj, 0, sizeof(lfs_pollfds_dynamic));
}

void lfs_pollfds_dynamic_destruct(lfs_pollfds_dynamic * obj)
{
    if (obj->pfds != NULL)
    {
        free(obj->pfds);
    }
    memset(obj, 0, sizeof * obj);
}