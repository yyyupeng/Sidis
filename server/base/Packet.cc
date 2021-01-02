#include "Packet.h"

int createNonblockAndCloseOnExec(sa_family_t family)
{
    int n = socket(family, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
    if(n < 0)
        fprintf(stderr, "socket error\n");
    return n;
}

void Bind(int sockfd, const SA *myaddr, socklen_t addrlen)
{
	if(bind(sockfd, myaddr, addrlen) < 0)
		fprintf(stderr, "bind error\n");
}

void Listen(int sockfd, int backlog)
{
	if(listen(sockfd, backlog) < 0)
		fprintf(stderr, "listen error\n");
}

int Accept(int sockfd, SA *cliaddr, socklen_t *addrlen)
{
	int n;
	if((n = accept(sockfd, cliaddr, addrlen)) < 0)
		fprintf(stderr, "accept error\n");
    return n;
}

int Fcntl(int fd, int cmd, long arg)
{
	int n;
	if((n = fcntl(fd, cmd, arg)) < 0)
		fprintf(stderr, "fcntl error\n");
	return n;
}

int setnonblocking(int sockfd)
{
	int val = Fcntl(sockfd, F_GETFL, 0);
	return Fcntl(sockfd, F_SETFL, val | O_NONBLOCK);
}

int Setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen)
{
	int n;
	if((n = setsockopt(sockfd, level, optname, optval, optlen)) < 0)
		fprintf(stderr, "set sockopt error\n");
	return n;
}

int Epoll_create(int size)
{
	int n;
	if((n = epoll_create(size)) < 0)
		fprintf(stderr, "epoll create error\n");
	return n;
}

int Epoll_ctl(int epfd, int op, int fd, SE *event)
{
	int n;
	if((n = epoll_ctl(epfd, op, fd, event)) < 0)
		fprintf(stderr, "epoll ctl error\n");
	return n;
}

int Epoll_wait(int epfd, SE *events, int maxevents, int timeout)
{
	int n;
	if((n = epoll_wait(epfd, events, maxevents, timeout)) < 0)
		fprintf(stderr, "epoll wait error\n");
	return n;
}

ssize_t readn(int sockfd, char *buf, ssize_t count)
{
    assert(sockfd != -1);
    assert(buf != NULL);
    assert(count != 0);
    char *bufp = buf;
    size_t nleft = 0;
    int nread = 0;
    while (nleft < count) 
    {
        nread = read(sockfd, bufp, count);
        if(nread < 0) 
        {
            if(errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK) 
                break;
            return -1;
        } 
        else if(nread == 0) 
        {
            std::cout << "Server is Over" << std::endl;
            return count;
        }
        nleft += nread;
    }
    return nleft;
}

ssize_t writen(int sockfd, char *buf, ssize_t count)
{
    assert(sockfd != -1);
    assert(buf != NULL);
    assert(count != 0);
    char *bufp = (char *)buf;
    size_t nleft = count;
    int nwrite = 0;
    while (nleft > 0) 
    {
        nwrite = write(sockfd, bufp, nleft);
        if(nwrite < 0) 
        {
            if(errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK) 
                continue;
            return -1;
        }
        if(nwrite == 0) continue;
        nleft -= nwrite;
        bufp += nwrite;
    }
    return count - nleft;
}

void Close(int sockfd)
{
	if(close(sockfd) < 0)
		fprintf(stderr, "close sockfd error\n");
}

void ShutdownWrite(int sockfd)
{
    if(shutdown(sockfd, SHUT_WR) < 0)
        fprintf(stderr, "shutdownWrite error\n");
}

void ToIp(char *buf, size_t size, const struct sockaddr *addr_)
{
    assert(size >= INET_ADDRSTRLEN);
    const struct sockaddr_in *addr_t = static_cast<const struct sockaddr_in*>(static_cast<const void*>(addr_));
    inet_ntop(AF_INET, &addr_t->sin_addr, buf, static_cast<socklen_t>(size));
}

void ToIpPort(char *buf, size_t size, const struct sockaddr *addr_)
{
    ToIp(buf, size, addr_);
    size_t end = strlen(buf);
    const struct sockaddr_in *addr_t = static_cast<const struct sockaddr_in*>(static_cast<const void*>(addr_));
    short port = ntohs(addr_t->sin_port);
    assert(size > end);
    snprintf(buf + end, size -end, ":%u", port);
}

void FromIpPort(const char *ip, short port, struct sockaddr_in *addr_)
{
    addr_->sin_family = AF_INET;
    addr_->sin_port = htons(port);
    if(inet_pton(AF_INET, ip, &addr_->sin_addr) <= 0)
        fprintf(stderr, "inet_pton error\n");
}

void my_err(const char *err_string,int line)
{
    fprintf(stderr,"line:%d ",line);
    perror(err_string);
    exit(1);
}
