#include "Acceptor.h"

Acceptor::Acceptor(EventLoop *loop_, const InetAddress& listenAddr)
:   loop(loop_),
    acceptSocket(createNonblockAndCloseOnExec(AF_INET)),
    acceptChannel(loop, acceptSocket.getSockfd()),
    listening(false)
{
    acceptSocket.setReuseAddr(true);
    acceptSocket.bindAddress(listenAddr);
    acceptChannel.setReadCallback(std::bind(&Acceptor::handleAccept, this));
    setNewConnectionCallback(std::bind(&Acceptor::NewConnection, this, _1, _2));
}

void Acceptor::listen()
{
    listening = true;
    acceptSocket.listen();
    acceptChannel.enableReading();
}

bool Acceptor::listenIng()
{
    return listening;
}

std::string Acceptor::getSocketInfo()
{
    return serverAddr.toIpPort();
}

void Acceptor::send(const std::string message)
{
    if(state == StateE::kConnected)
        sendInLoop(message);
}

void Acceptor::sendInLoop(const std::string message)
{
    ssize_t nwrite = 0;
    if(state == StateE::kDisconnected)
    {
        fprintf(stderr, "disconnected, give up writing\n");
        return;
    }
    if(!channelAccept->isWriting() && outputBuffer.readableBytes() == 0)
    {
        nwrite = write(channelAccept->getSockfd(), message.data(), message.size());
        if(nwrite >= 0)
        {
            std::cout << "write complete" << std::endl;
        }
        else
        {
            nwrite = 0;
            if(errno == EWOULDBLOCK)
            {
                fprintf(stderr, "unknown error\n");
            }
        }
    }
    assert(nwrite >= 0);
    if((size_t)nwrite < message.size())
    {
        outputBuffer.append(message.data() + nwrite, message.size() - nwrite);
        if(!channelAccept->isWriting())
        {
            channelAccept->enableWriting();
        }
    }
}

void Acceptor::shutdown()
{
    if(state == StateE::kConnected)
    {
        setState(StateE::kDisconnecting);
        loop->runInLoop(std::bind(&Acceptor::shutdownInLoop, this));
    }
}

void Acceptor::shutdownInLoop()
{
    if(!channelAccept->isWriting())
    {
        acceptSocket.shutdownWrite();
    }
}

void Acceptor::setChannel(const SP_Channel& Channel_)
{
    channelAccept = Channel_;
    setState(StateE::kConnected);
    channelAccept->setReadCallback(std::bind(&Acceptor::handleRead, this));
    channelAccept->setWriteCallback(std::bind(&Acceptor::handleWrite, this));
    channelAccept->enableReading();
}

void Acceptor::removeChannel()
{
    std::bind(&Acceptor::handleClose, this);
}

void Acceptor::NewConnection(int sockfd, const InetAddress& peeraddr)
{
    serverAddr = peeraddr;
    shared_from_this()->setCloseCallback(std::bind(&Acceptor::handleClose, this));
    setChannel(SP_Channel(new Channel(loop, sockfd)));
}

void Acceptor::setNewConnectionCallback(const NewConnectionCallback&& cb)
{
    newConnectionCallback = cb;
}

void Acceptor::setConnectionCallback(const ConnectionCallback&& cb)
{
    connectionCallback = cb;
}

void Acceptor::setMessageCallback(const MessageCallback&& cb)
{
    messageCallback = cb;
}

void Acceptor::setCloseCallback(const CloseCallback&& cb)
{
    closeCallback = cb;
}

void Acceptor::setState(StateE s)
{
    state = s;
}

void Acceptor::handleAccept()
{
    InetAddress peerAddr;
    int connfd = acceptSocket.accept();
    std::cout << connfd << std::endl;
    if(connfd >= 0)
    {
        if(newConnectionCallback)
        {
            newConnectionCallback(connfd, peerAddr);
        }
        else
        {
            Close(connfd);
        }
    }
}

void Acceptor::handleRead()
{
    cout << channelAccept->getSockfd() << endl;
    ssize_t n = inputBuffer.readFd(channelAccept->getSockfd());
    if(n > 0)
    {
        messageCallback(shared_from_this(), &inputBuffer, n);
    }
    else if(n == 0)
    {
        handleClose();
    }
    else
    {
        fprintf(stderr, "handleRead error\n");
        return;
    }
}

void Acceptor::handleWrite()
{
    if(channelAccept->isWriting())
    {
        ssize_t n = write(channelAccept->getSockfd(), outputBuffer.peek(), outputBuffer.readableBytes());
        if(n > 0)
        {
            outputBuffer.retrieve(n);
            if(outputBuffer.readableBytes() == 0)
            {
                channelAccept->disableWriting();
                if(state == StateE::kDisconnecting)
                {
                    shutdownInLoop();
                }
            }
        }
        else
        {
            fprintf(stderr, "handleWrite error\n");
        }
    }
    else
    {
        std::cout << "Connection fd = " << channelAccept->getSockfd() << " is down, no more writing" << std::endl;
    }
}

void Acceptor::handleClose()
{
    assert(state == StateE::kConnected || state == StateE::kDisconnecting);
    setState(StateE::kDisconnected);
    channelAccept->disableAll();
    connectionCallback(shared_from_this());
    loop->removeChannel(channelAccept.get());
}

