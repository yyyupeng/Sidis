#pragma once

#include "../base/Packet.h"
#include "Channel.h"

class Channel;
typedef std::vector<SE> EventList;
typedef std::map<int, Channel*> ChannelMap;
typedef std::vector<class Channel*> ChannelList;

class Epoll 
{
public:
    Epoll() = default;
    ~Epoll() {}
    void setEpollfd(int epfd);
    void epoll(ChannelList *activeChannel);
    void fillActiveChannel(int numEvents, ChannelList *activeChanel);
    void updateChannel(Channel *channel_);
    void removeChannel(Channel *channel_);

private:
    int epollfd;
    EventList events;
    ChannelMap channel;
};
