#include "Epoll.h"

void Epoll::setEpollfd(int epfd)
{
    epollfd = epfd;
}

void Epoll::epoll(ChannelList *activeChannel) 
{
    int numEvents = Epoll_wait(epollfd,  &*events.begin(), static_cast<int>(events.size()), -1);
    assert(numEvents >= 0);
    if(numEvents > 0) 
    {
        std::cout << "Events Happened: " << numEvents << std::endl;
        fillActiveChannel(numEvents, activeChannel);
        if (numEvents == events.size()) 
        {
            events.resize(events.size() * 2);
        }
    } 
    else if(numEvents == 0)
        std::cout << "No things Happened" << std::endl;
    else
        abort();
  return;
}

void Epoll::fillActiveChannel(int numEvents, ChannelList *activeChannelList) 
{
    assert(numEvents <= events.size());
    for (int i = 0; i < numEvents; i++) 
    {
        Channel *channel_ = static_cast<Channel *>(events[i].data.ptr);
        int sockfd = channel_->getSockfd();
        ChannelMap::const_iterator it = channel.find(sockfd);
        assert(it != channel.end());
        assert(channel_ == it->second);
        channel_->setRevents(events[i].events);
        activeChannelList->push_back(channel_);
  }
}

void Epoll::updateChannel(Channel *channel_)
{
    if(channel_->getIndex() < 0) 
    {
        struct epoll_event efd;
        efd.events = channel_->getEvents();
        efd.data.fd = channel_->getSockfd();
        efd.data.ptr = channel_;
        int ret = Epoll_ctl(epollfd, EPOLL_CTL_ADD, channel_->getSockfd(), &efd);
        std::cout << "update channel: " << strerror(errno) << std::endl;
        assert(ret == 0);
        events.push_back(efd);
        channel_->setIndex((int)events.size() - 1);
        channel[channel_->getSockfd()] = channel_;
    }
    else 
    {
        assert(channel.find(channel_->getSockfd()) != channel.end());
        assert(channel[channel_->getSockfd()] == channel_);
        int index = channel_->getIndex();
        assert(index >= 0 && index <= (int)events.size());
        struct epoll_event &efd = events[index];
        Channel *channelptr = (Channel *)efd.data.ptr;
        assert(channelptr->getSockfd() == channel_->getSockfd() || channelptr->getSockfd() == -1);
        efd.events = channel_->getEvents();
        int ret = Epoll_ctl(epollfd, EPOLL_CTL_MOD, channel_->getSockfd(), &efd);
        std::cout << "update channel: " << strerror(errno) << std::endl;
        assert(ret == 0);
    }
}

void Epoll::removeChannel(Channel *channel_) 
{
    assert(channel.find(channel_->getSockfd()) != channel.end());
    assert(channel[channel_->getSockfd()] == channel_);
    assert(channel_->isNoneEvent());
    int index = channel_->getIndex();
    assert(index >= 0 && index <= (int)events.size());
    const struct epoll_event &efd = events[index];
    (void)efd;
    assert(efd.events == channel_->getEvents());
    size_t ret = channel.erase(channel_->getSockfd());
    assert(ret == 1);
    (void)ret;
    if(index == (int)events.size() - 1) 
    {
        events.pop_back();
    }
    else 
    {
        int channelEnd = events.back().data.fd;
        // Swap the element you want to delete with the tail element
        std::iter_swap(events.begin() + index, events.end() - 1);

        if(channelEnd < 0) 
        {
            channelEnd = -channelEnd - 1;
        }
        // Update the location of the current element
        channel[channelEnd]->setIndex(index);
        events.pop_back();
    }
    int res = Epoll_ctl(epollfd, EPOLL_CTL_DEL, channel_->getSockfd(), const_cast<struct epoll_event *>(&efd));
    assert(res == 0);
}
