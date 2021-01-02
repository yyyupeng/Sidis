#include "db/Server.h"

int main()
{
    EventLoop loop;
    InetAddress listenaddr(9527);
    Server server(&loop,listenaddr);
    loop.loop();
    return 0;
}

