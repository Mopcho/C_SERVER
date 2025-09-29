#include <stdio.h>
#include <stdlib.h>
#include "LFS/server.h"
#include <signal.h>
#include <string.h>

void no_zombie_processes()
{
    struct sigaction sa = {0};
    sa.sa_handler = SIG_DFL;
    sa.sa_flags = SA_NOCLDWAIT;
    if (sigaction(SIGCHLD, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(-1);
    }
}

int main(int argc, char *argv[])
{
    no_zombie_processes();

    if (argc != 2)
    {
        printf("Wrong count of arguments passed. Expected 1.");
        exit(1);
    }

    const size_t result = lfs_listen(NULL, argv[1]);

    return (int)result;
}
