#include "../client/Client.h"

int main()
{
    Client client(9527, "127.0.0.1");
    client.Connect();
    client.handle();
    return 0;
}

