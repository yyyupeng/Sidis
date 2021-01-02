#include "Server.h"

Server::Server(EventLoop *loop_, const InetAddress& listenAddr)
:   loop(loop_),
    accepter(SP_Acceptor(new Acceptor(loop_, listenAddr)))
{
    accepter->setConnectionCallback(std::bind(&Server::onConnection, this, _1));
    accepter->setMessageCallback(std::bind(&Server::onMessage, this, _1, _2, _3));
    accepter->listen();
    Init();
}

Server::~Server()
{
    while(waitpid(-1, NULL, WNOHANG) != -1);
}

void Server::Init()
{
    for(int i = 0; i < DBNUM; ++i)
        database.emplace_back(SP_Database(new Database()));
    dbIndex = 0;
    database[dbIndex]->rdbLoad(dbIndex);
    cmdDict.insert(make_pair("bgsave", std::bind(&Server::bgsaveCommand, this, _1)));
    cmdDict.insert(make_pair("select", std::bind(&Server::selectCommand, this, _1)));
    cmdDict.insert(make_pair("pexpire", std::bind(&Server::pexpireCommand, this, _1)));
    cmdDict.insert(make_pair("set", std::bind(&Server::setCommand, this, _1)));
    cmdDict.insert(make_pair("get", std::bind(&Server::getCommand, this, _1)));
    cmdDict.insert(make_pair("rpush", std::bind(&Server::rpushCommand, this, _1)));
    cmdDict.insert(make_pair("rpop", std::bind(&Server::rpopCommand, this, _1)));
    cmdDict.insert(make_pair("hset", std::bind(&Server::hsetCommand, this, _1)));
    cmdDict.insert(make_pair("hget", std::bind(&Server::hgetCommand, this, _1)));
    cmdDict.insert(make_pair("hgetall", std::bind(&Server::hgetallCommand, this, _1)));
    cmdDict.insert(make_pair("sadd", std::bind(&Server::saddCommand, this, _1)));
    cmdDict.insert(make_pair("smembers", std::bind(&Server::smembersCommand, this, _1)));
    cmdDict.insert(make_pair("zadd", std::bind(&Server::zaddCommand, this, _1)));
    cmdDict.insert(make_pair("zcard", std::bind(&Server::zcardCommand, this, _1)));
}

void Server::onConnection(const SP_Acceptor& conn)
{
    std::cout << "new Connection" << std::endl;
}

void Server::onMessage(const SP_Acceptor& conn, Buffer *buf, ssize_t n)
{
    std::string res = cmdRequest(buf);
    conn->send(res);
}

string Server::cmdRequest(Buffer *buf)
{
    std::string res;
    std::string msg = buf->retrieveAllAsString();
    std::cout << "request: " << msg << std::endl;
    
    std::istringstream ss(msg);
    std::string cmd, key, ObjKey, ObjValue;
    ss >> cmd;
    assert(!cmd.empty());
    if(cmd == "bgsave")
    {
        auto it = cmdDict.find(cmd);
        if(it == cmdDict.end())
            return dbStatus::notFound("Not Found Command").toString();
        else
        {
            VctS vs = {cmd};
            res = it->second(std::move(vs));
        }
    }
    else if(cmd == "select")
    {
        auto it = cmdDict.find(cmd);
        if(it == cmdDict.end())
            return dbStatus::notFound("Not Found Command").toString();
        else
        {
            ss >> key;
            VctS vs = {cmd, key};
            res = it->second(std::move(vs));
        }
    }
    else if(cmd == "pexpire")
    {
        auto it = cmdDict.find(cmd);
        if(it == cmdDict.end())
            return dbStatus::notFound("Not Found Command").toString();
        else
        {
            ss >> key;
            ss >> ObjKey;
            VctS vs = {cmd, key, ObjKey};
            res = it->second(std::move(vs));
        }
    }
    else if(cmd == "set")
    {
        auto it = cmdDict.find(cmd);
        if(it == cmdDict.end())
            return dbStatus::notFound("Not Found Command").toString();
        else
        {
            ss >> key;
            ss >> ObjKey;
            VctS vs = {cmd, key, ObjKey};
            res = it->second(std::move(vs));
        }
    }
    else if(cmd == "get")
    {
        auto it = cmdDict.find(cmd);
        if(it == cmdDict.end())
            return dbStatus::notFound("Not Found Command").toString();
        else
        {
            ss >> key;
            VctS vs = {cmd, key};
            res = it->second(std::move(vs));
        }
    }
    else if(cmd == "rpush")
    {
        auto it = cmdDict.find(cmd);
        if(it == cmdDict.end())
            return dbStatus::notFound("Not Found Command").toString();
        else
        {
            ss >> key;
            ss >> ObjKey;
            VctS vs = {cmd, key, ObjKey};
            res = it->second(std::move(vs));
        }
    }
    else if(cmd == "rpop")
    {
        auto it = cmdDict.find(cmd);
        if(it == cmdDict.end())
            return dbStatus::notFound("Not Found Command").toString();
        else
        {
            ss >> key;
            VctS vs = {cmd, key};
            res = it->second(std::move(vs));
        }
    }
    else if(cmd == "hset")
    {
        auto it = cmdDict.find(cmd);
        if(it == cmdDict.end())
            return dbStatus::notFound("Not Found Command").toString();
        else
        {
            ss >> key;
            ss >> ObjKey;
            ss >> ObjValue;
            VctS vs = {cmd, key, ObjKey, ObjValue};
            res = it->second(std::move(vs));
        }
    }
    else if(cmd == "hget")
    {
        auto it = cmdDict.find(cmd);
        if(it == cmdDict.end())
            return dbStatus::notFound("Not Found Command").toString();
        else
        {
            ss >> key;
            ss >> ObjKey;
            VctS vs = {cmd, key, ObjKey};
            res = it->second(std::move(vs));
        }
    }
    else if(cmd == "hgetall")
    {
        auto it = cmdDict.find(cmd);
        if(it == cmdDict.end())
            return dbStatus::notFound("Not Found Command").toString();
        else
        {
            ss >> key;
            VctS vs = {cmd, key};
            res = it->second(std::move(vs));
        }
    }
    else if(cmd == "sadd")
    {
        auto it = cmdDict.find(cmd);
        if(it == cmdDict.end())
            return dbStatus::notFound("Not Found Command").toString();
        else
        {
            ss >> key;
            ss >> ObjKey;
            VctS vs = {cmd, key, ObjKey};
            res = it->second(std::move(vs));
        }

    }
    else if(cmd == "smembers")
    {
        auto it = cmdDict.find(cmd);
        if(it == cmdDict.end())
            return dbStatus::notFound("Not Found Command").toString();
        else
        {
            ss >> key;
            VctS vs = {cmd, key};
            res = it->second(std::move(vs));
        }
    }
    else if(cmd == "zadd")
    {
        auto it = cmdDict.find(cmd);
        if(it == cmdDict.end())
            return dbStatus::notFound("Not Found Command").toString();
        else
        {
            ss >> key;
            ss >> ObjKey;
            ss >> ObjValue;
            VctS vs = {cmd, key, ObjKey, ObjValue};
            res = it->second(std::move(vs));
        }
    }
    else if(cmd == "zcard")
    {
        auto it = cmdDict.find(cmd);
        if(it == cmdDict.end())
            return dbStatus::notFound("Not Found Command").toString();
        else
        {
            ss >> key;
            VctS vs = {cmd, key};
            res = it->second(std::move(vs));
        }
    }
    else 
    {
        return dbStatus::notFound("Not Found Command").toString();
    }
    return res;
}

bool Server::checkSaveCondition()
{
    LL save_interval = getTime() - lastsave;
    if(save_interval > dbObject::rdbDefaultTime)
    {
        std::cout << "bgsaveing" << std::endl;
        rdbSave();
        lastsave = getTime();
        return true;
    }
    return false;
}

void Server::rdbSave()
{
    pid_t pid = fork();
    if(pid == 0)
    {
        std::cout << "child process" << std::endl;
        char buf[1024] = {0};
        string path = getcwd(buf, 1024);
        assert(!path.empty());
        path += "/dump.rdb";
        
        std::ofstream out;
        out.open(path, std::ios::out | std::ios::app | std::ios::binary);
        if(!out.is_open())
        {
            std::cout << "RDB持久化失败" << std::endl;
            abort();
        }

        string str;
        str = saveHead();
        for(int i = 0; i < DBNUM; ++i)
        {
            if(database[i]->getKeySize() == 0)  //db为空
                continue;

            str += saveSelectDb(i);

            if(database[i]->getKeyStringSize() != 0)
            {
                str += saveType(dbObject::dbString);
                auto it = database[i]->getKeyStringObj().begin();
                for(; it != database[i]->getKeyStringObj().end(); it++)
                {
                    str += saveExpiredTime(database[i]->getKeyExpiredTime(dbObject::dbString, it->first));
                    str += saveKV(it->first, it->second);
                }
            }
            if(database[i]->getKeyListSize() != 0)
            {
                str += saveType(dbObject::dbList);
                auto it = database[i]->getKeyListObj().begin();
                for(; it != database[i]->getKeyListObj().end(); it++)
                {
                    str += saveExpiredTime(database[i]->getKeyExpiredTime(dbObject::dbList, it->first));
                    auto iter = it->second.begin();
                    string tmp = '!' + to_string(it->second.size());
                    for(; iter != it->second.end(); iter++)
                    {
                        tmp += '!' + to_string(iter->size()) + '$' + iter->c_str();
                    }
                    str += '!' + to_string(it->first.size()) + '#' + it->first.c_str() + tmp;
                }
            }
            if(database[i]->getKeyHashSize() != 0)
            {
                str += saveType(dbObject::dbHash);
                auto it = database[i]->getKeyHashObj().begin();
                for(; it != database[i]->getKeyHashObj().end(); it++)
                {
                    str += saveExpiredTime(database[i]->getKeyExpiredTime(dbObject::dbHash, it->first));
                    auto iter = it->second.begin();
                    string tmp = '!' + to_string(it->second.size());
                    for(; iter != it->second.end(); iter++)
                    {
                        tmp += saveKV(iter->first, iter->second);
                    }
                    str += '!' + to_string(it->first.size()) + '#' + it->first.c_str() + tmp;
                }
            }
            if(database[i]->getKeySetSize() != 0)
            {
                str += saveType(dbObject::dbSet);
                auto it = database[i]->getKeySetObj().begin();
                for(; it != database[i]->getKeySetObj().end(); it++)
                {
                    str += saveExpiredTime(database[i]->getKeyExpiredTime(dbObject::dbSet, it->first));
                    auto iter = it->second.begin();
                    string tmp = '!' + to_string(it->second.size());
                    for(; iter != it->second.end(); iter++)
                    {
                        tmp += '!' + to_string(iter->size()) + '$' + iter->c_str();
                    }
                    str += '!' + to_string(it->first.size()) + '#' + it->first.c_str() + tmp;
                }

            }
            if(database[i]->getKeyZsetSize() != 0)
            {
                str += saveType(dbObject::dbZset);
                auto it = database[i]->getKeyZsetObj().begin();
                for(; it != database[i]->getKeyZsetObj().end(); it++)
                {
                    str += saveExpiredTime(database[i]->getKeyExpiredTime(dbObject::dbZset, it->first));
                    string tmp = '!' + to_string(it->second->getLength());
                    rangespec spec(DBL_MIN, DBL_MAX);
                    vector<skiplistNode*> vecSkip(it->second->getNodeInRange(spec)); 
                    for(int i = 0; i < (int)vecSkip.size(); ++i)
                    {
                        tmp += saveKV(vecSkip[i]->obj, to_string(vecSkip[i]->score));
                    }
                    str += '!' + to_string(it->first.size()) + '#' + it->first.c_str() + tmp;
                }
            }
            str.append("EOF");
            out.write(str.c_str(), str.size());
        }
        out.close();
        exit(0);
    }
    else if(pid > 0)
        std::cout << "parent process" << std::endl;
    else
        std::cout << "fork error" << std::endl;
}

string Server::bgsaveCommand(VctS&& argv)
{
    if(argv.size() != 1)
        return dbStatus::IOError("Parameter error").toString();
    bool flag = checkSaveCondition();
    if(flag)
        return dbStatus::Ok().toString();
    else
        return dbStatus::IOError("bgsave error").toString();
}

string Server::selectCommand(VctS&& argv)
{
    if(argv.size() != 2)
        return dbStatus::IOError("Parameter error").toString();
    int index = atoi(argv[1].c_str());
    dbIndex = index - 1;
    database[dbIndex]->rdbLoad(dbIndex);   //加载rdb文件
    return dbStatus::Ok().toString();
}

string Server::pexpireCommand(VctS&& argv)
{
    if(argv.size() != 3)
        return dbStatus::IOError("Parameter error").toString();
    bool flag = database[dbIndex]->setPexpireTime(dbObject::dbString, argv[1], atoll(argv[2].c_str()));
    if(flag)
        return dbStatus::Ok().toString();
    else
        return dbStatus::IOError("pexpire error").toString();
}

string Server::setCommand(VctS&& argv) 
{
    if(argv.size() != 3)
        return dbStatus::IOError("Parameter error").toString();
    bool flagExpire = database[dbIndex]->judgeKeyExpiredTime(dbObject::dbString, argv[1]);
    if(flagExpire)
        database[dbIndex]->delKey(dbObject::dbString, argv[1]);

    bool flag = database[dbIndex]->addKey(dbObject::dbString, dbObject::dbEncodingString, argv[1], argv[2], dbObject::defaultObjValue);
    if(flag)
        return dbStatus::Ok().toString();
    else
        return dbStatus::IOError("set error").toString();
}

string Server::getCommand(VctS&& argv)
{
    if(argv.size() != 2)
        return dbStatus::IOError("Parameter error").toString();
    bool flagExpire = database[dbIndex]->judgeKeyExpiredTime(dbObject::dbString, argv[1]);
    if(flagExpire)
    {
        database[dbIndex]->delKey(dbObject::dbString, argv[1]);
        return dbStatus::IOError("Empty Content").toString();
    }
    
    string res = database[dbIndex]->getKey(dbObject::dbString, argv[1]);
    if(res.empty())
        return dbStatus::IOError("Empty Content").toString();
    else
        return res;
}

string Server::rpushCommand(VctS&& argv)
{
    if(argv.size() < 3) 
        return dbStatus::IOError("Parameter error").toString();
    bool flag;
    for(int i = 2; i < (int)argv.size(); ++i)
        flag = database[dbIndex]->addKey(dbObject::dbList, dbObject::dbEncodingLinkedlist, argv[1], argv[i], dbObject::defaultObjValue);
    if(flag)
        return dbStatus::Ok().toString();
    else
        return dbStatus::IOError("rpush error").toString();
}

string Server::rpopCommand(VctS&& argv)
{
    if(argv.size() != 2)
        return dbStatus::IOError("Parameter error").toString();
    string res = database[dbIndex]->rpopList(argv[1]);
    if(res.empty())
        return dbStatus::IOError("rpop error").toString();
    else
        return '+' + res;
}

string Server::hsetCommand(VctS&& argv)
{
    if(argv.size() != 4)
        return dbStatus::IOError("Parameter error").toString();
    bool flag = database[dbIndex]->addKey(dbObject::dbHash, dbObject::dbEncodingRbt, argv[1], argv[2], argv[3]);
    if(flag)
        return dbStatus::Ok().toString();
    else
        return dbStatus::IOError("hset error").toString();
}

string Server::hgetCommand(VctS&& argv)
{
    if(argv.size() != 3)
        return dbStatus::IOError("Parameter error").toString();
    auto tmpHash = database[dbIndex]->getKeyHashObj();
    auto it = tmpHash.find(argv[1]);
    if(it == tmpHash.end())
        return dbStatus::notFound("Not Found").toString();
    else
    {
        std::map<string, string>::iterator iter;
        iter = it->second.find(argv[2]);
        if(iter == it->second.end())
            return dbStatus::notFound("Not Found").toString();
        else
            return '+' + iter->second;
    }
}

string Server::hgetallCommand(VctS&& argv)
{
    if(argv.size() != 2)
        return dbStatus::IOError("Parameter error").toString();
    string res = database[dbIndex]->getKey(dbObject::dbHash, argv[1]);
    if(res.empty())
        return dbStatus::IOError("Empty Content").toString();
    else
        return res;
}

string Server::saddCommand(VctS&& argv)
{
    if(argv.size() != 3)
        return dbStatus::IOError("Parameter error").toString();
    bool flag = database[dbIndex]->addKey(dbObject::dbSet, dbObject::dbEncodingHt, argv[1], argv[2], dbObject::defaultObjValue);
    if(flag)
        return dbStatus::Ok().toString();
    else
        return dbStatus::IOError("sadd error").toString();
}

string Server::smembersCommand(VctS&& argv)
{
    if(argv.size() != 2)
        return dbStatus::IOError("Parameter error").toString();
    string res = database[dbIndex]->getKey(dbObject::dbSet, argv[1]);
    if(res.empty())
        return dbStatus::IOError("Empty Content").toString();
    else
        return res;
}

string Server::zaddCommand(VctS&& argv)
{
    if(argv.size() != 4)
        return dbStatus::IOError("Parameter error").toString();
    bool flag = database[dbIndex]->addKey(dbObject::dbZset, dbObject::dbEncodingSkiplist, argv[1], argv[2], argv[3]);
    if(flag)
        return dbStatus::Ok().toString();
    else
        return dbStatus::IOError("hset error").toString();
}

string Server::zcardCommand(VctS&& argv)
{
    if(argv.size() != 2)
        return dbStatus::IOError("Parameter error").toString();
    auto tmpZset = database[dbIndex]->getKeyZsetObj();
    auto it = tmpZset.find(argv[1]);
    if(it == tmpZset.end())
        return dbStatus::notFound("Not Found").toString();
    else
        return '+' + to_string(it->second->getLength());
}

//毫秒级
LL Server::getTime()
{
    struct timeval now;
    assert(gettimeofday(&now, NULL) != -1);
    return (LL)now.tv_sec * 1000 + now.tv_usec / 1000;
}

string Server::saveHead()
{
    string tmp = "REDIS0006";
    return tmp;
}

string Server::saveSelectDb(const int index)
{
    return "SD" + to_string(index);
}

string Server::saveExpiredTime(const LL expiredTime)
{
    return "ST" + to_string(expiredTime);
}

string Server::saveType(const int type)
{
    return string("^" + to_string(type));
}

string Server::saveKV(const string& key, const string& value)
{
    char buf[1024];
    sprintf(buf,"!%d#%s!%d$%s", (int)key.size(), key.c_str(), (int)value.size(), value.c_str());
    return string(buf);
}
