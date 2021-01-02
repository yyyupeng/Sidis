#include "Database.h"

bool Database::addKey(const int type, const int encoding, const string& key, const string& objKey, const string& objValue)
{
    /* LL expired = expiredTime + getTime(); */
    if(type == dbObject::dbString)
    {
        auto it = _String.find(key);
        if(it == _String.end())
        {
            _String.insert(make_pair(key, objKey));
            /* _StringExpire.insert(make_pair(key, expired)); */
            _StringLru.set(key, objKey);
        }
        else
        {
            _String[key] = objKey;
            /* _StringExpire[key] = expired; */
            _StringLru.set(key, objKey);
        }
    }
    else if(type == dbObject::dbList)
    {
        auto it = _List.find(key);
        if(it == _List.end())
        {
            list<string, __gnu_cxx::__pool_alloc<string>> tmp;
            tmp.emplace_back(objKey);
            _List.insert(make_pair(key, tmp));
            /* _ListExpire.insert(make_pair(key, expired)); */
            _ListLru.set(key, tmp);
        }
        else
        {
            auto iter = _List.find(key);
            list<string>::iterator tit = iter->second.begin();
            list<string, __gnu_cxx::__pool_alloc<string>> tmp;
            while(tit != iter->second.end())
            {
                tmp.emplace_back(*tit);
                tit++;
            }
            tmp.emplace_back(objKey);
            //更新
            iter->second.emplace_back(objKey);
            /* _ListExpire[key] = expired; */ 
            _ListLru.set(key, tmp);
        }
    }
    else if(type == dbObject::dbHash)
    {
        auto it = _Hash.find(key);
        if(it == _Hash.end())
        {
            map<string, string, less<string>, __gnu_cxx::__pool_alloc<pair<const string, string>>> tmp;
            tmp.insert(make_pair(objKey, objValue));
            _Hash.insert(make_pair(key, tmp));
            /* _HashExpire.insert(make_pair(key, expired)); */
            _HashLru.set(key, tmp);
        }
        else
        {
            auto iter = _Hash.find(key);
            map<string, string>::iterator tit = iter->second.begin();
            map<string, string, less<string>, __gnu_cxx::__pool_alloc<pair<const string, string>>> tmp;
            while(tit != iter->second.end())
            {
                tmp.insert(make_pair(tit->first, tit->second));
                tit++;
            }
            /* tmp.insert(make_pair(objKey, objValue)); */
            tmp[objKey] = objValue;
            /* iter->second.insert(make_pair(objKey, objValue)); */
            iter->second[objKey] = objValue;
            /* _HashExpire[key] = expired; */
            _HashLru.set(key, tmp);
        }
    }
    else if(type == dbObject::dbSet)
    {
        auto it = _Set.find(key);
        if(it == _Set.end())
        {
            unordered_set<string, std::hash<string>, std::equal_to<string>, __gnu_cxx::__pool_alloc<string>> tmp;
            tmp.insert(objKey);
            _Set.insert(make_pair(key, tmp));
            /* _SetExpire.insert(make_pair(key, expired)); */
            _SetLru.set(key, tmp);
        }
        else
        {
            auto iter = _Set.find(key);
            unordered_set<string>::iterator tit = iter->second.begin();
            unordered_set<string, std::hash<string>, std::equal_to<string>, __gnu_cxx::__pool_alloc<string>> tmp;
            while(tit != iter->second.end())
            {
                tmp.insert(*tit);
                tit++;
            }
            tmp.insert(objKey);
            iter->second.insert(objKey);
            /* _SetExpire[key] = expired; */
            _SetLru.set(key, tmp);
        }
    }
    else if(type == dbObject::dbZset)
    {
        auto it = _Zset.find(key);
        if(it == _Zset.end())
        {
            SP_Skiplist skip(new Skiplist());
            skip->insertNode(objKey, atoi(objValue.c_str()));
            _Zset.insert(make_pair(key, skip));
            /* _ZsetExpire.insert(make_pair(key, expired)); */
            _ZsetLru.set(key, skip);
        }
        else
        {
            auto iter = _Zset.find(key);
            iter->second->insertNode(objKey, atoi(objValue.c_str()));
            /* _ZsetExpire[key] = expired; */
            _ZsetLru.set(key, iter->second);
        }
    }
    else
    {
        std::cout << "Unknown type" << std::endl;
        return false;
    }
    std::cout << "Add key executed successfully" << std::endl;
    return true;
}

bool Database::delKey(const int type, const string& key)
{
    if(type == dbObject::dbString)
    {
        auto it = _String.find(key);
        if(it != _String.end())
        {
            _String.erase(key);
            _StringExpire.erase(key);
        }
        else
        {
            std::cout << "Not Found" << std::endl;
            return false;
        }
    }
    else if(type == dbObject::dbList)
    {
        auto it = _List.find(key);
        if(it != _List.end())
        {
            _List.erase(key);
            _ListExpire.erase(key);
        }
        else
        {
            std::cout << "Not Found" << std::endl;
            return false;
        }
    }
    else if(type == dbObject::dbHash)
    {
        auto it = _Hash.find(key);
        if(it != _Hash.end())
        {
            _Hash.erase(key);
            _HashExpire.erase(key);
        }
        else
        {
            std::cout << "Not Found" << std::endl;
            return false;
        }
    }
    else if(type == dbObject::dbSet)
    {
        auto it = _Set.find(key);
        if(it != _Set.end())
        {
            _Set.erase(key);
            _SetExpire.erase(key);
        }
        else
        {
            std::cout << "Not Found" << std::endl;
            return false;
        }
    }
    else if(type == dbObject::dbZset)
    {
        auto it = _Zset.find(key);
        if(it != _Zset.end())
        {
            _Zset.erase(key);
            _ZsetExpire.erase(key);
        }
        else
        {
            std::cout << "Not Found" << std::endl;
            return false;
        }
    }
    else
    {
        std::cout << "Unknown type" << std::endl;
        return false;
    }
    std::cout << "Delete key executed successfully" << std::endl;
    return true;
}

string Database::getKey(const int type, const string& key)
{
    string res;
    if(!judgeKeyExpiredTime(type, key))
    {
        if(type == dbObject::dbString)
        {
            auto it = _String.find(key);
            if(it == _String.end())
                res = dbStatus::notFound("Not Found").toString();
            else
                res = '+' + it->second;
        }
        else if(type == dbObject::dbList)
        {
            auto it = _List.find(key);
            if(it == _List.end())
                res = dbStatus::notFound("Not Found").toString();
            else
            {
                std::list<string>::iterator iter;
                for(iter = it->second.begin(); iter != it->second.end(); iter++)
                    res += *iter + ' ';
                res = '+' + res;
            }
        }
        else if(type == dbObject::dbHash)
        {
            auto it = _Hash.find(key);
            if(it == _Hash.end())
                res = dbStatus::notFound("Not Found").toString();
            else
            {
                std::map<string, string>::iterator iter;
                for(iter = it->second.begin(); iter != it->second.end(); iter++)
                    res += iter->first + ':' + iter->second + ' ';
                res = '+' + res;
            }
        }
        else if(type == dbObject::dbSet)
        {
            auto it = _Set.find(key);
            if(it == _Set.end())
                res = dbStatus::notFound("Not Found").toString();
            else
            {
                std::unordered_set<string>::iterator iter;
                for(iter = it->second.begin(); iter != it->second.end(); iter++)
                    res += *iter + ' ';
                res = '+' + res;
            }
        }
        else if(type == dbObject::dbZset)
        {
            auto it = _Zset.find(key);
            if(it == _Zset.end())
                res = dbStatus::notFound("Not Found").toString();
            else
            {
                rangespec spec(DBL_MIN, DBL_MAX);
                vector<skiplistNode*> vecSkip(it->second->getNodeInRange(spec)); 
                for(int i = 0; i < (int)vecSkip.size(); ++i)
                    res += vecSkip[i]->obj + ':' + to_string(vecSkip[i]->score) + ' ';
                res = '+' + res;
            }
        }
        else
        {
            std::cout << "Unknown type" << std::endl;
            res = dbStatus::IOError("Unknown type").toString();
        }
    }
    else
    {
        delKey(type, key);
        res = "The key has expired and will be deleted";
    }
    return res;
}

LL Database::getKeyExpiredTime(const int type, const string& key)
{
    LL tmp;
    if(type == dbObject::dbString)
    {
        Expire::iterator it = _StringExpire.find(key);
        if(it != _StringExpire.end())
        {
            tmp = it->second;
        }
        else
        {
            tmp = -1;
        }
    }
    else if(type == dbObject::dbList)
    {
        Expire::iterator it = _ListExpire.find(key);
        if(it != _ListExpire.end())
        {
            tmp = it->second;
        }
        else
        {
            tmp = -1;
        }
    }
    else if(type == dbObject::dbHash)
    {
        Expire::iterator it = _HashExpire.find(key);
        if(it != _HashExpire.end())
        {
            tmp = it->second;
        }
        else
        {
            tmp = -1;
        }
    }
    else if(type == dbObject::dbSet)
    {
        Expire::iterator it = _SetExpire.find(key);
        if(it != _SetExpire.end())
        {
            tmp = it->second;
        }
        else
        {
            tmp = -1;
        }
    }
    else if(type == dbObject::dbZset)
    {
        Expire::iterator it = _ZsetExpire.find(key);
        if(it != _ZsetExpire.end())
        {
            tmp = it->second;
        }
        else
        {
            tmp = -1;
        }
    }
    else
    {
        tmp = -2;
        std::cout << "Unknown type" << std::endl;
    }
    return tmp;
}

bool Database::judgeKeyExpiredTime(const int type, const string& key)
{
    LL expired = getKeyExpiredTime(type, key);
    if(expired < 0)
        return false;
    else
    {
        LL now = getTime();
        if(now > expired)
            return true;
        else
            return false;
    }
}

bool Database::setPexpireTime(const int type, const string& key, LL expiredTime)
{
    if(type == dbObject::dbString)
    {
        auto it = _String.find(key);
        if(it != _String.end())
        {
            LL now = expiredTime + getTime();
            _StringExpire[key] = now;
            return true;
        }
    }
    else if(type == dbObject::dbList)
    {
        auto it = _List.find(key);
        if(it != _List.end())
        {
            LL now = expiredTime + getTime();
            _ListExpire[key] = now;
            return true;
        }
    }
    else if(type == dbObject::dbHash)
    {
        auto it = _Hash.find(key);
        if(it != _Hash.end())
        {
            LL now = expiredTime + getTime();
            _HashExpire[key] = now;
            return true;
        }
    }
    else if(type == dbObject::dbSet)
    {
        auto it = _Set.find(key);
        if(it != _Set.end())
        {
            LL now = expiredTime + getTime();
            _SetExpire[key] = now;
            return true;
        }
    }
    else if(type == dbObject::dbZset)
    {
        auto it = _Zset.find(key);
        if(it != _Zset.end())
        {
            LL now = expiredTime + getTime();
            _ZsetExpire[key] = now;
            return true;
        }
    }

    return false;
}

const string Database::rpopList(const string& key)
{
    auto iter = _List.find(key);
    if(iter != _List.end())
    {
        string res = iter->second.back();
        iter->second.pop_back();
        return res;
    }
    else
        return dbStatus::notFound("Not Found").toString();
}

String& Database::getKeyStringObj()
{
    return _String;
}

List& Database::getKeyListObj()
{
    return _List;
}

Hash& Database::getKeyHashObj()
{
    return _Hash;
}

Set& Database::getKeySetObj()
{
    return _Set;
}

Zset& Database::getKeyZsetObj()
{
    return _Zset;
}

int Database::getKeySize()
{
    return getKeyStringSize() + getKeyListSize() + getKeyHashSize() + getKeySetSize() + getKeyZsetSize();
}

int Database::getKeyStringSize()
{
    return _String.size();
}

int Database::getKeyListSize()
{
    return _List.size();
}

int Database::getKeyHashSize()
{
    return _Hash.size();
}

int Database::getKeySetSize()
{
    return _Set.size();
}

int Database::getKeyZsetSize()
{
    return _Zset.size();
}

void Database::rdbLoad(int index)
{
    char tmp[1024] = {0};
    string path = getcwd(tmp, 1024);
    path += "/dump.rdb";
    int fd = open(path.c_str(), O_CREAT | O_RDONLY, 0644);
    assert(fd != -1);

    struct stat buf;
    fstat(fd, &buf);
    if(buf.st_size == 0)    return;

    char *addr = static_cast<char*>(mmap(NULL, buf.st_size, PROT_READ, MAP_SHARED, fd, 0));
    if(addr == MAP_FAILED)
    {
        std::cout << "rdbSave error" << std::endl;
        abort();
    }
    close(fd);

    string data(addr, addr + buf.st_size);
    assert(munmap(addr, buf.st_size) != -1);
    cout << "data.size: " << data.size() << "\ndata: " << data << endl;

    int pos1 = 0, pos2 = 0;
    int dbNum = 0;
    do
    {
        pos1 = data.find("SD", pos2);
        if(pos1 == data.npos)   //该db没有数据要载入
            return;
        pos2 = data.find('^', pos1);
        dbNum = atoi(interceptString(data, pos1 + 2, pos2).c_str());
    
    }while(dbNum != index);

    int end = data.find("SD", pos2);
    if(end == data.npos)
        end = data.find("EOF");
    while(pos1 < end && pos2 << end)
    {
        pos2 = data.find('^', pos1);
        pos1 = data.find("ST", pos2);
        int type = atoi(interceptString(data, pos2 + 1, pos1).c_str());
        /* pos2 = data.find('!', pos1); */
        /* LL expiredTime = atoi(interceptString(data, pos1 + 2, pos2).c_str()); */
     
        if(type == dbObject::dbString)
        {
            do
            {
                pos2 = data.find('!', pos1);
                LL expiredTime = atoi(interceptString(data, pos1 + 2, pos2).c_str());
                pos1 = data.find('#', pos2);
                int keyLen = atoi(interceptString(data, pos2 + 1, pos1).c_str());
                string key = data.substr(pos1 + 1, keyLen);
                pos2 = data.find('!', pos1);
                pos1 = data.find('$', pos2);
                int valueLen = atoi(interceptString(data, pos2 + 1, pos1).c_str());
                string value = data.substr(pos1 + 1, valueLen);
                /* _String.insert(make_pair(key, value)); */
                addKey(dbObject::dbString, dbObject::dbEncodingString, key, value, dbObject::defaultObjValue);
                if(expiredTime > 0 && expiredTime > getTime())
                    setPexpireTime(dbObject::dbString, key, expiredTime);
                pos1 += 1 + valueLen;

            }while(data.substr(pos1, 2) == "ST");
            continue;
        }
        if(type == dbObject::dbList)
        {
            do
            {
                pos2 = data.find('!', pos1);
                LL expiredTime = atoi(interceptString(data, pos1 + 2, pos2).c_str());
                pos1 = data.find('#', pos2);
                int keyLen = atoi(interceptString(data, pos2 + 1, pos1).c_str());
                string key = data.substr(pos1 + 1, keyLen);
                pos2 = data.find('!', pos1);
                pos1 = data.find('!', pos2 + 1);
                int valueSize = atoi(interceptString(data, pos2 + 1, pos1).c_str());
                int valueLen = 0;
                while(valueSize--)
                {
                    pos2 = pos1;
                    pos1 = data.find('$', pos2);
                    valueLen = atoi(interceptString(data, pos2 + 1, pos1).c_str());
                    string value = data.substr(pos1 + 1, valueLen);
                    if(valueSize > 1)
                        pos1 = data.find('!', pos2 + 1);
                    addKey(dbObject::dbList, dbObject::dbEncodingLinkedlist, key, value, dbObject::defaultObjValue);
                }
                if(expiredTime > 0 && expiredTime > getTime())
                    setPexpireTime(dbObject::dbList, key, expiredTime);
                pos1 += 1 + valueLen;

            }while(data.substr(pos1, 2) == "ST");
            continue;
        }
        if(type == dbObject::dbHash)
        {
            do
            {
                pos2 = data.find('!', pos1);
                LL expiredTime = atoi(interceptString(data, pos1 + 2, pos2).c_str());
                pos1 = data.find('#', pos2);
                int keyLen = atoi(interceptString(data, pos2 + 1, pos1).c_str());
                string key = data.substr(pos1 + 1, keyLen);
                pos2 = data.find('!', pos1);
                pos1 = data.find('!', pos2 + 1);
                int valueSize = atoi(interceptString(data, pos2 + 1, pos1).c_str());
                int valueLen = 0;
                while(valueSize--)
                {
                    pos2 = data.find('#', pos1);
                    int valueKeyLen = atoi(interceptString(data, pos1 + 1, pos2).c_str());
                    string valueKey = data.substr(pos2 + 1, valueKeyLen);
                    pos1 = data.find('!', pos2);
                    pos2 = pos1;
                    pos1 = data.find('$', pos2);
                    valueLen = atoi(interceptString(data, pos2 + 1, pos1).c_str());
                    string value = data.substr(pos1 + 1, valueLen);
                    if(valueSize > 1)
                        pos1 = data.find('!', pos2 + 1);
                    addKey(dbObject::dbHash, dbObject::dbEncodingRbt, key, valueKey, value);
                }
                if(expiredTime > 0 && expiredTime > getTime())
                    setPexpireTime(dbObject::dbHash, key, expiredTime);
                pos1 += 1 + valueLen;

            }while(data.substr(pos1, 2) == "ST");
            continue;
        }
        if(type == dbObject::dbSet)
        {
            do
            {
                pos2 = data.find('!', pos1);
                LL expiredTime = atoi(interceptString(data, pos1 + 2, pos2).c_str());
                pos1 = data.find('#', pos2);
                int keyLen = atoi(interceptString(data, pos2 + 1, pos1).c_str());
                string key = data.substr(pos1 + 1, keyLen);
                pos2 = data.find('!', pos1);
                pos1 = data.find('!', pos2 + 1);
                int valueSize = atoi(interceptString(data, pos2 + 1, pos1).c_str());
                int valueLen = 0;
                while(valueSize--)
                {
                    pos2 = pos1;
                    pos1 = data.find('$', pos2);
                    valueLen = atoi(interceptString(data, pos2 + 1, pos1).c_str());
                    string value = data.substr(pos1 + 1, valueLen);
                    if(valueSize > 1)
                        pos1 = data.find('!', pos2 + 1);
                    addKey(dbObject::dbSet, dbObject::dbEncodingHt, key, value, dbObject::defaultObjValue);
                }
                if(expiredTime > 0 && expiredTime > getTime())
                    setPexpireTime(dbObject::dbSet, key, expiredTime);
                pos1 += 1 + valueLen;

            }while(data.substr(pos1, 2) == "ST");
            continue;
        }
        if(type == dbObject::dbZset)
        {
            do
            {
                pos2 = data.find('!', pos1);
                LL expiredTime = atoi(interceptString(data, pos1 + 2, pos2).c_str());
                pos1 = data.find('#', pos2);
                int keyLen = atoi(interceptString(data, pos2 + 1, pos1).c_str());
                string key = data.substr(pos1 + 1, keyLen);
                pos2 = data.find('!', pos1);
                pos1 = data.find('!', pos2 + 1);
                int valueSize = atoi(interceptString(data, pos2 + 1, pos1).c_str());
                int valueLen = 0;
                while(valueSize--)
                {
                    pos2 = data.find('#', pos1);
                    int valueKeyLen = atoi(interceptString(data, pos1 + 1, pos2).c_str());
                    string valueKey = data.substr(pos2 + 1, valueKeyLen);
                    pos1 = data.find('!', pos2);
                    pos2 = pos1;
                    pos1 = data.find('$', pos2);
                    valueLen = atoi(interceptString(data, pos2 + 1, pos1).c_str());
                    string value = data.substr(pos1 + 1, valueLen);
                    if(valueSize > 1)
                        pos1 = data.find('!', pos2 + 1);
                    addKey(dbObject::dbHash, dbObject::dbEncodingRbt, key, valueKey, value);
                }
                if(expiredTime > 0 && expiredTime > getTime())
                    setPexpireTime(dbObject::dbHash, key, expiredTime);
                pos1 += 1 + valueLen;

            }while(data.substr(pos1, 2) == "ST");
            continue;
        }
    }
}

string Database::interceptString(const string& ss, int pos1, int pos2)
{
    if(pos1 > pos2)
        swap(pos1, pos2);
    return ss.substr(pos1, pos2 - pos1);
}

//毫秒级
LL Database::getTime()
{
    struct timeval now;
    assert(gettimeofday(&now, NULL) != -1);
    return (LL)now.tv_sec * 1000 + now.tv_usec / 1000;
}
