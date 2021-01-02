#pragma once

#include "../server/base/Packet.h"

class Client
{
public:
    Client(int port_, string ip_);
    ~Client();
    void Connect();
    void handle();
    void handleRequest(const std::string& buf);

private:
    const int port;
    const std::string ip;
    int connfd;

    void send(char *buf);
    void recv(char *buf);
};

