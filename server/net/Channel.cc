#include "Channel.h"

const int Channel::NoneEvent = 0;
const int Channel::ReadEvent = EPOLLIN | EPOLLPRI;
const int Channel::WriteEvent = EPOLLOUT;

Channel::Channel(EventLoop *loop_, const int sockfd)
:   loop(loop_),
    fd(sockfd),
    events(0),
    revents(0),
    index(-1)
{
}

void Channel::handleEvent() 
{
    if(revents & (EPOLLIN | EPOLLPRI)) 
    {
        if(readcallback) readcallback();
        if(sreadcallback) sreadcallback();
    }
    if(revents & EPOLLOUT)
        if(writecallback) writecallback();
}

void Channel::setReadCallback(const CallBack&& cb)
{
    readcallback = cb;
}

void Channel::setsReadCallback(const CallBack&& cb)
{
    sreadcallback = cb;
}

void Channel::setWriteCallback(const CallBack&& cb)
{
    writecallback = cb;
}

int Channel::getSockfd()
{
    return fd;
}

void Channel::setSockfd(int sockfd)
{
    fd = sockfd;
}

int Channel::getEvents()
{
    return events;
}

void Channel::setRevents(int rev)
{
    revents = rev;
}

int Channel::getIndex()
{
    return index;
}

void Channel::setIndex(int idx)
{
    index = idx;
}

bool Channel::isNoneEvent()
{
    return events == NoneEvent;
}

bool Channel::isWriting()
{
    return events & WriteEvent;
}

void Channel::enableReading() 
{
    events |= ReadEvent;
    update();
}

void Channel::enableWriting() 
{
    events |= WriteEvent;
    update();
}

void Channel::disableWriting() 
{
    events &= ~WriteEvent;
    update();
}

void Channel::disableAll() 
{
    events = NoneEvent;
    update();
}

void Channel::update() 
{ 
    loop->updateChannel(this); 
}

