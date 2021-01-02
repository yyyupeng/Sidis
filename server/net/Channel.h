#pragma once

#include "../base/Head.h"
#include "EventLoop.h"

class EventLoop;
typedef std::function<void()> CallBack;

class Channel
{
public:
    Channel(EventLoop *loop, const int sockfd);
    void handleEvent();
    void setReadCallback(const CallBack&& cb);
    void setsReadCallback(const CallBack&& cb);
    void setWriteCallback(const CallBack&& cb); 
    int getSockfd();
    void setSockfd(int sockfd);
    int getEvents();
    void setRevents(int revt);
    int getIndex();
    void setIndex(int ind);
    bool isNoneEvent();
    bool isWriting();
    void enableReading(); 
    void enableWriting(); 
    void disableWriting(); 
    void disableAll(); 

private:
    EventLoop *loop;
    static const int NoneEvent;
    static const int ReadEvent;
    static const int WriteEvent;
    int fd;
    int index;
    int events;
    int revents;
    CallBack readcallback;
    CallBack sreadcallback;
    CallBack writecallback;

    void update();
};
