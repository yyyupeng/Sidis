#include "InetAddress.h"

static const in_addr_t KInaddrAny = INADDR_ANY;
static const in_addr_t KInaddrLoopback = INADDR_LOOPBACK;

InetAddress::InetAddress(short port, bool loopbackOnly)
{
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    in_addr_t ip = loopbackOnly ? KInaddrLoopback : KInaddrAny;
    addr.sin_addr.s_addr = htonl(ip);
    addr.sin_port = htons(port);
}

InetAddress::InetAddress(std::string& ip, short port)
{
    memset(&addr, 0, sizeof(addr));

}

std::string InetAddress::toIp() const
{
    char buf[64] = "";
    ToIp(buf, sizeof(buf), getSockAddr());
    return buf;
}

std::string InetAddress::toIpPort() const
{
    char buf[64] = "";
    ToIpPort(buf, sizeof(buf), getSockAddr());
    return buf;
}

const struct sockaddr* InetAddress::getSockAddr() const
{
    return static_cast<const struct sockaddr*>(static_cast<const void*>(&addr));
}
