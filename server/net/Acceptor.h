#pragma once

#include "../base/Packet.h"
#include "Socket.h"
#include "Buffer.h"
#include "Channel.h"
#include "InetAddress.h"
#include "EventLoop.h"

class EventLoop;
class InetAddress;
class Acceptor;
typedef std::shared_ptr<Channel> SP_Channel;
typedef std::shared_ptr<Acceptor> SP_Acceptor;

class Acceptor : public std::enable_shared_from_this<Acceptor>
{
public:
    typedef std::function<void(int sockfd, const InetAddress&)> NewConnectionCallback;
    typedef std::function<void(const SP_Acceptor& conn)> ConnectionCallback;
    typedef std::function<void(const SP_Acceptor& conn, Buffer *buf, ssize_t n)> MessageCallback;
    typedef std::function<void()> CloseCallback;

    Acceptor(EventLoop *loop_, const InetAddress& listenAddr);
    void listen();
    bool listenIng();
    std::string getSocketInfo();
    void send(const std::string message);
    void shutdown();
    void setChannel(const SP_Channel& Channel_);
    void removeChannel();
    void NewConnection(int sockfd, const InetAddress& peeraddr);
    void setNewConnectionCallback(const NewConnectionCallback&& cb);
    void setConnectionCallback(const ConnectionCallback&& cb);
    void setMessageCallback(const MessageCallback&& cb);
    void setCloseCallback(const CloseCallback&& cb);

private:
    enum StateE { kDisconnected, kConnecting, kConnected, kDisconnecting };
    StateE state;
    EventLoop *loop;
    InetAddress serverAddr;
    class Socket acceptSocket;
    class Channel acceptChannel;
    SP_Channel channelAccept;
    bool listening;
    Buffer inputBuffer;
    Buffer outputBuffer;
    NewConnectionCallback newConnectionCallback;
    ConnectionCallback connectionCallback;
    MessageCallback messageCallback;
    CloseCallback closeCallback;

    void setState(StateE s);
    void handleAccept();
    void handleRead();
    void handleWrite();
    void handleClose();
    void sendInLoop(const std::string message);
    void shutdownInLoop();
};
