#pragma once

#include "../net/Acceptor.h"
#include "Database.h"
#include "dbStatus.h"

class Database;
typedef std::shared_ptr<Database> SP_Database;
typedef std::vector<std::string> VctS;

class Server
{
public:
    Server(EventLoop *loop_, const InetAddress& listenAddr);
    ~Server();
    void Init();
    void onConnection(const SP_Acceptor& conn);
    void onMessage(const SP_Acceptor& conn, Buffer *buf, ssize_t n);
    string cmdRequest(Buffer *buf);
    bool checkSaveCondition();
    void rdbSave();

private:
    EventLoop *loop;
    SP_Acceptor accepter;
    int dbIndex;
    std::vector<SP_Database> database;
    std::unordered_map<string, function<string(VctS&&)>> cmdDict;
    LL lastsave = 0;

    string bgsaveCommand(VctS&&);
    string selectCommand(VctS&&);
    string pexpireCommand(VctS&&);
    string setCommand(VctS&&);
    string getCommand(VctS&&);
    string rpushCommand(VctS&&);
    string rpopCommand(VctS&&);
    string hsetCommand(VctS&&);
    string hgetCommand(VctS&&);
    string hgetallCommand(VctS&&);
    string saddCommand(VctS&&);
    string smembersCommand(VctS&&);
    string zaddCommand(VctS&&);
    string zcardCommand(VctS&&);
    LL getTime();
    string saveHead();
    string saveSelectDb(const int index);
    string saveExpiredTime(const LL ExpiredTime);
    string saveType(const int type);
    string saveKV(const string& key, const string& value);
};

