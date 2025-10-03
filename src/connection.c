#include "LFS/connection.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void lfs_connection_dynamic_init(lfs_connection_dynamic * obj)
{
    memset(obj, 0, sizeof(lfs_connection_dynamic));
}

void lfs_connection_dynamic_add(lfs_connection_dynamic * obj, lfs_connection toadd)
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
        lfs_connection * new_memory_block = malloc(new_cap * sizeof(lfs_connection));
        if (new_memory_block == NULL)
        {
            perror("malloc");
            exit(-1);
        }

        // move from old to new memory block
        memcpy(new_memory_block, obj->connections, obj->size * sizeof(lfs_connection));
        free(obj->connections);
        obj->connections = new_memory_block;
        obj->cap = new_cap;
    }

    obj->connections[obj->size] = toadd;
    obj->size++;
}

void lfs_connection_dynamic_remove(lfs_connection_dynamic * obj, int sockfd_toremove)
{
    for (int i = 0; i < obj->size; i++)
    {
        if (obj->connections[i].sockfd == sockfd_toremove)
        {
            memset(&obj->connections[i], 0, sizeof(lfs_connection));
        }
    }
}

void lfs_connection_dynamic_destruct(lfs_connection_dynamic * obj)
{
    if (obj->connections != NULL)
    {
        free(obj->connections);
    }
    memset(obj, 0, sizeof * obj);
}