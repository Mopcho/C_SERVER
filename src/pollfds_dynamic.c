#include "LFS/pollfds_dynamic.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void pollfds_dynamic_add(pollfds_dynamic * obj, struct pollfd toadd)
{
    if (obj->size == obj->cap)
    {
        printf("grow \n");
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

void pollfds_dynamic_remove(pollfds_dynamic * obj, int sockfd_toremove)
{
    for (int i = 0; i < obj->size; i++)
    {
        if (obj->pfds[i].fd == sockfd_toremove)
        {
            memset(&obj->pfds[i], 0, sizeof(struct pollfd));
        }
    }
}

void pollfds_dynamic_init(pollfds_dynamic * obj)
{
    memset(obj, 0, sizeof(pollfds_dynamic));
}

void pollfds_dynamic_destruct(pollfds_dynamic * obj)
{
    if (obj->pfds != NULL)
    {
        free(obj->pfds);
    }
    memset(obj, 0, sizeof * obj);
}