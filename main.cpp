#include <csignal>
#include <cstring>
#include <string>

#include "LFS/server.hpp"
#include "LFS/client.hpp"
#include "LFS/udp_yapper.hpp"
#include "LFS/udp_listener.hpp"

int main(int argc, char* argv[])
{
    if (argc < 4)
    {
        printf("Wrong count of arguments passed. Expected at least 3 [command(connect/listen/yap/listen_yap)] [host] [port].");
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

        server.get()->handle("/api/users", [](lfs::Request* req, lfs::Response* res)
        {
            res->send(R"({"user": {"id": 1042,"username": "mopcho","email": "mopcho@example.com"})");
        });

        server.get()->handle("/ui/users", [](lfs::Request* req, lfs::Response* res)
        {
            res->set_header("Content-Type", "text/html; charset=UTF-8");
            res->send("<html>"
  "<head>"
    "<title>This is the title of the webpage!</title>"
  "</head>"
  "<body>"
    "<h1> Some Heading </h1>"
    "<p>This is an example paragraph. Anything in the <strong>body</strong> tag will appear on the page, just like this <strong>p</strong> tag and its contents.</p>"
  "</body>"
"</html>");
        });

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