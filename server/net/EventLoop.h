#pragma once

#include "../base/Packet.h"
#include "Epoll.h"

class Epoll;
class Channel;
typedef std::function<void()> Functor;
typedef std::shared_ptr<Epoll> SP_Epoll;
typedef std::vector<class Channel*> ChannelList;

class EventLoop 
{
public:
    EventLoop();
    ~EventLoop();
    bool quit;
    SP_Epoll epoll;
    void loop();
    void setquit();
    void runInLoop(const Functor& cb);
    void updateChannel(Channel *channel);
    void removeChannel(Channel *channel);

private:
    bool looping;
    ChannelList activeChannels;
};
