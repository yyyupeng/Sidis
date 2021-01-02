#pragma once

#include "../base/Packet.h"

class InetAddress
{
public:
    explicit InetAddress(const struct sockaddr_in addr_) : addr(addr_) {}
    explicit InetAddress(short port = 0, bool loopbackOnly = false);
    InetAddress(std::string& ip, short port);
    std::string toIp() const;
    std::string toIpPort() const;
    const struct sockaddr *getSockAddr() const;

private:
    struct sockaddr_in addr;
};

