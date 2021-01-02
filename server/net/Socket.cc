#include "Socket.h"

Socket::~Socket()
{
    Close(sockfd);
}

int Socket::getSockfd() const
{
    return sockfd;
}

void Socket::bindAddress(const InetAddress& localaddr)
{
    Bind(sockfd, localaddr.getSockAddr(), sizeof(SA));
}

void Socket::listen()
{
    Listen(sockfd, LISTENQ);
}

int Socket::accept()
{
    struct sockaddr_in addr;
    socklen_t len = sizeof(struct sockaddr_in);
    memset(&addr, 0, sizeof(addr));
    int connfd = Accept(sockfd, (struct sockaddr*)&addr, &len);
    return connfd;
}

void Socket::shutdownWrite()
{
    ShutdownWrite(sockfd);
}

void Socket::setReuseAddr(bool on)
{
    int optval = on ? 1 : 0;
    Setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, static_cast<socklen_t>(sizeof(optval)));
}

/*
void Socket::setReusePort(bool on)
{
    int optval = on ? 1 : 0;
    Setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &optval, static_cast<socklen_t>(sizeof(optval)));
}*/
