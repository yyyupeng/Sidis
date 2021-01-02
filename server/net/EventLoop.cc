#include "EventLoop.h"

EventLoop::EventLoop()
:   looping(false),
    epoll(new Epoll)
{
    int epfd = Epoll_create(1000);
    epoll->setEpollfd(epfd);
}

EventLoop::~EventLoop()
{
    assert(!looping);
}

void EventLoop::loop() 
{
    assert(!looping);
    looping = true;
    quit = false;
    while(!quit) 
    {
        activeChannels.clear();
        
        epoll->epoll(&activeChannels);
        for(ChannelList::iterator it = activeChannels.begin(); it != activeChannels.end(); it++)
        {
            (*it)->handleEvent();
        }
    }
    looping = false;
}

void EventLoop::setquit()
{
    quit = true;
}

void EventLoop::runInLoop(const Functor& cb)
{
    cb();
}

void EventLoop::updateChannel(Channel *channel) 
{
    epoll->updateChannel(channel);
}

void EventLoop::removeChannel(Channel *channel)
{
    epoll->removeChannel(channel);
}
