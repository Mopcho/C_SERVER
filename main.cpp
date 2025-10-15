#include <csignal>
#include <cstring>
#include <string>

#include "LFS/server.hpp"
#include "LFS/client.hpp"
#include "LFS/udp_yapper.hpp"
#include "LFS/udp_listener.hpp"

void no_zombie_processes() // TODO: We can move this as an option to the server
{
    struct sigaction sa {};
    sa.sa_handler = SIG_DFL;
    sa.sa_flags = SA_NOCLDWAIT;
    if (sigaction(SIGCHLD, &sa, nullptr) == -1)
    {
        perror("sigaction");
        exit(-1);
    }
}

int main(int argc, char* argv[])
{
    if (argc < 4)
    {
        printf("Wrong count of arguments passed. Expected at least 3 [command(connect/listen)] [host] [port].");
        exit(1);
    }

    std::string command = argv[1];
    std::string host = argv[2];
    std::string port = argv[3];

    std::transform(command.begin(), command.end(), command.begin(), [](unsigned char c){ return std::tolower(c); });

    int return_code = 0;
    if (command == "connect")
    {
        auto client = std::make_unique<lfs::Client>(host, port);
        client->connect();
    }
    else if (command == "listen")
    {
        auto server = std::make_unique<lfs::Server>(host, port);
        return_code = server->listen();
    }
    else if (command == "yap")
    {
        return_code = lfs::UDP::yap(host.c_str(), port.c_str(), argv[4], strlen(argv[4]));
    }
    else if (command == "listen_yap")
    {
        return_code = lfs::UDP::listen(host.c_str(), port.c_str());
    }
    else
    {
        printf("not a valid command");
    }

    return return_code;
}