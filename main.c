#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <poll.h>

#include "LFS/server.h"
#include "LFS/client.h"
#include "LFS/pollfds_dynamic.h"
#include "LFS/udp_yapper.h"
#include "LFS/udp_listener.h"

void no_zombie_processes() // TODO: We can move this as an option to the server
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

/**
 * Converts a string to its lowercase version and puts it inside the given buffer
 * @param src source string
 * @param buf destination buffer
 * @param buf_size buffer size ( with null terminated char space included )
 */
void str_tolower(const char* src, char* buf, size_t buf_size) // TODO: Move this to some kind of utils
{
    size_t index = 0;
    while (src[index] != '\0' && index < buf_size - 1)
    {
        buf[index] = (char)tolower(src[index]);
        index++;
    }
    buf[index] = '\0';
}

int main(int argc, char* argv[])
{
    if (argc < 4)
    {
        printf("Wrong count of arguments passed. Expected at least 3 [command(connect/listen)] [host] [port].");
        exit(1);
    }

    const char* command = argv[1];
    const char* host = argv[2];
    const char* port = argv[3];

    char command_lower[15];
    str_tolower(command, command_lower, sizeof command_lower / sizeof command_lower[0]);

    int result = 0;
    if (strcmp(command_lower, "connect") == 0)
    {
        result = lfs_connect(host, port);
    }
    else if (strcmp(command_lower, "listen") == 0)
    {
        result = lfs_listen(host, port);
    }
    else if (strcmp(command_lower, "yap") == 0)
    {
        const char * msg = argv[4];
        size_t msg_len = strlen(msg);
        result = lfs_yap(host, port, msg, msg_len);
    }
    else if (strcmp(command_lower, "listen_yap") == 0)
    {
        result = lfs_udplisten(host, port);
    }
    else
    {
        printf("not a valid command");
    }

    return result;
}