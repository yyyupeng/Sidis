#pragma once

#include "../base/Packet.h"
#include "InetAddress.h"

class InetAddress;

class Socket
{
public:
    explicit Socket(int sockfd_) : sockfd(sockfd_) {}
    ~Socket();
    int getSockfd() const;
    void bindAddress(const InetAddress& localaddr);
    void listen();
    int accept();
    void shutdownWrite();
    void setReuseAddr(bool on);
    /* void setReusePort(bool on); */

private:
    const int sockfd;
};

