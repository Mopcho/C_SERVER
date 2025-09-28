#include <stdio.h>
#include <stdlib.h>
#include "LFS/server.h"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Wrong count of arguments passed. Expected 1.");
        exit(1);
    }

    int result = lfs_listen(NULL, argv[1]);

    return result;
}
