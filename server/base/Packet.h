#pragma once

#include "Head.h"

typedef struct sockaddr SA;
typedef struct epoll_event SE;

 //全部为包裹函数
int createNonblockAndCloseOnExec(sa_family_t family);
void Bind(int sockfd, const SA *myaddr, socklen_t addrlen);
void Listen(int sockfd, int backlog);
int Accept(int sockfd, SA *cliaddr, socklen_t *addrlen);
int Fcntl(int fd, int cmd, long arg);
int setnonblocking(int sockfd);
int Setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
int Epoll_create(int size);
int Epoll_ctl(int epfd, int op, int fd, SE *event);
int Epoll_wait(int epfd, SE *events, int maxevents, int timeout);
ssize_t readn(int sockfd, char *buf, ssize_t count);
ssize_t writen(int sockfd, char *buf, ssize_t count);
void Close(int fd);
void ShutdownWrite(int sockfd);
void ToIp(char *buf, size_t size, const struct sockaddr *addr_);
void ToIpPort(char *buf, size_t size, const struct sockaddr *addr_);
void FromIpPort(const char *ip, short port, struct sockaddr_in *addr_);
void my_err(const char *err_string, int line);

