#include "Client.h"

Client::Client(int port_, string ip_) : port(port_), ip(ip_) {}

Client::~Client() { close(connfd); }

void Client::Connect()
{
    struct sockaddr_in serv_addr;
    memset(&serv_addr,0,sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = inet_addr(ip.c_str());

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    connfd = connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(struct sockaddr_in));
    if(connfd < 0)
    {
        std::cout << "connect error" << std::endl;
        abort();
    }
    connfd = sockfd;
    cout << connfd << endl;
}

void Client::handle()
{
    char buf[1024] = {0};
    int flag = 0;
    std::cout << "Sidis> ";
    while(fgets(buf, sizeof(buf), stdin) != NULL)
    {
        std::string tmp(buf, buf + strlen(buf));
        int pos = tmp.find_last_of('\n');
        tmp = tmp.substr(0, pos);
        if(tmp == "quit")
            break;
        handleRequest(tmp);

        std::istringstream ss(tmp);
        std::string sel;
        ss >> sel;
        if(sel == "select")
        {
            pos = tmp.find_first_of(' ');
            flag = atoi(tmp.substr(pos, tmp.size()).c_str());
        }
        if(flag == 0)
            std::cout << "Sidis> ";
        else
            std::cout << "Sidis" << "[" << flag << "]> ";
    }
}

void Client::handleRequest(const std::string& buf)
{
    std::istringstream ss(buf);
    std::string cmd, key, ObjKey, ObjValue;
    char buffer[1024] = {0};
    
    ss >> cmd;
    assert(!cmd.empty());
    if(cmd == "bgsave")
    {
        snprintf(buffer, sizeof(buffer), "%s", cmd.c_str());
        send(buffer);
        memset(buffer,0,1024);
        recv(buffer);
    }
    else if(cmd == "select")
    {
        ss >> key;
        assert(!key.empty());
        snprintf(buffer, sizeof(buffer), "%s %s", cmd.c_str(), key.c_str());
        send(buffer);
        memset(buffer,0,1024);
        recv(buffer);
    }
    else if(cmd == "pexpire")
    {
        ss >> key;
        assert(!key.empty());
        ss >> ObjKey;
        assert(!ObjKey.empty());
        snprintf(buffer, sizeof(buffer), "%s %s %s", cmd.c_str(), key.c_str(), ObjKey.c_str());
        send(buffer);
        memset(buffer,0,1024);
        recv(buffer);
    }
    else if(cmd == "set")
    {
        ss >> key;
        assert(!key.empty());
        ss >> ObjKey;
        assert(!ObjKey.empty());
        snprintf(buffer, sizeof(buffer), "%s %s %s", cmd.c_str(), key.c_str(), ObjKey.c_str());
        send(buffer);
        memset(buffer,0,1024);
        recv(buffer);
    }
    else if(cmd == "get")
    {
        ss >> key;
        assert(!key.empty());
        snprintf(buffer, sizeof(buffer), "%s %s", cmd.c_str(), key.c_str());
        send(buffer);
        memset(buffer,0,1024);
        recv(buffer);
    }
    else if(cmd == "rpush")
    {
        ss >> key;
        assert(!key.empty());
        ss >> ObjKey;
        assert(!ObjKey.empty());
        snprintf(buffer, sizeof(buffer), "%s %s %s", cmd.c_str(), key.c_str(), ObjKey.c_str());
        send(buffer);
        memset(buffer,0,1024);
        recv(buffer);
    }
    else if(cmd == "rpop")
    {
        ss >> key;
        assert(!key.empty());
        snprintf(buffer, sizeof(buffer), "%s %s", cmd.c_str(), key.c_str());
        send(buffer);
        memset(buffer,0,1024);
        recv(buffer);
    }
    else if(cmd == "hset")
    {
        ss >> key;
        assert(!key.empty());
        ss >> ObjKey;
        assert(!ObjKey.empty());
        ss >> ObjValue;
        assert(!ObjValue.c_str());
        snprintf(buffer, sizeof(buffer), "%s %s %s %s", cmd.c_str(), key.c_str(), ObjKey.c_str(), ObjValue.c_str());
        send(buffer);
        memset(buffer,0,1024);
        recv(buffer);
    }
    else if(cmd == "hget")
    {
        ss >> key;
        assert(!key.empty());
        ss >> ObjKey;
        assert(!ObjKey.empty());
        snprintf(buffer, sizeof(buffer), "%s %s %s", cmd.c_str(), key.c_str(), ObjKey.c_str());
        send(buffer);
        memset(buffer,0,1024);
        recv(buffer);
    }
    else if(cmd == "hgetall")
    {
        ss >> key;
        assert(!key.empty());
        snprintf(buffer, sizeof(buffer), "%s %s", cmd.c_str(), key.c_str());
        send(buffer);
        memset(buffer,0,1024);
        recv(buffer);
    }
    else if(cmd == "sadd")
    {
        ss >> key;
        assert(!key.empty());
        ss >> ObjKey;
        assert(!ObjKey.empty());
        snprintf(buffer, sizeof(buffer), "%s %s %s", cmd.c_str(), key.c_str(), ObjKey.c_str());
        send(buffer);
        memset(buffer,0,1024);
        recv(buffer);
    }
    else if(cmd == "smembers")
    {
        ss >> key;
        assert(!key.empty());
        snprintf(buffer, sizeof(buffer), "%s %s", cmd.c_str(), key.c_str());
        send(buffer);
        memset(buffer,0,1024);
        recv(buffer);
    }
    else if(cmd == "zadd")
    {
        ss >> key;
        assert(!key.empty());
        ss >> ObjKey;
        assert(!ObjKey.empty());
        ss >> ObjValue;
        assert(!ObjValue.c_str());
        snprintf(buffer, sizeof(buffer), "%s %s %s %s", cmd.c_str(), key.c_str(), ObjKey.c_str(), ObjValue.c_str());
        send(buffer);
        memset(buffer,0,1024);
        recv(buffer);
    }
    else if(cmd == "zcard")
    {
        ss >> key;
        assert(!key.empty());
        snprintf(buffer, sizeof(buffer), "%s %s", cmd.c_str(), key.c_str());
        send(buffer);
        memset(buffer,0,1024);
        recv(buffer);
    }
    else
    {
        std::cout << "cmd error" << std::endl;
        return;
    }
}

void Client::send(char *buf)
{
    int ret = writen(connfd, buf, strlen(buf));
    assert(ret == strlen(buf));
}

void Client::recv(char *buf)
{
    int ret = read(connfd, buf, 1024);
    /* assert(ret != -1); */
    std::cout << buf + 1 << std::endl;
}
