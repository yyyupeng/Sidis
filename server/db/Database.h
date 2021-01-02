#pragma once

/* #include "../base/Head.h" */
#include "../base/LRUCache.h"
#include "dbObject.h"
#include "dbStatus.h"
#include "Skiplist.h"

template <typename T1, typename T2>
using Dict = unordered_map<T1, T2, std::hash<T1>, std::equal_to<T1>, __gnu_cxx::__pool_alloc<pair<const T1, T2>>>;

typedef std::shared_ptr<Skiplist> SP_Skiplist;
typedef Dict<string, string> String;
typedef Dict<string, list<string, __gnu_cxx::__pool_alloc<string>>> List;
typedef Dict<string, map<string, string, less<string>, __gnu_cxx::__pool_alloc<pair<const string, string>>>> Hash;
typedef Dict<string, unordered_set<string, std::hash<string>, std::equal_to<string>, __gnu_cxx::__pool_alloc<string>>> Set;
typedef Dict<string, SP_Skiplist> Zset;
typedef Dict<string, LL> Expire;

class Database
{
public:
    Database() {}
    ~Database() {}
    bool addKey(const int type, const int encoding, const string& key, const string& objKey, const string& objValue);
    bool delKey(const int type, const string& key);
    string getKey(const int type, const string& key);
    LL getKeyExpiredTime(const int type, const string& key);
    bool judgeKeyExpiredTime(const int type, const string& key);
    bool setPexpireTime(const int type, const string& key, LL expiredTime);
    const string rpopList(const string& key);
    String& getKeyStringObj();
    List& getKeyListObj();
    Hash& getKeyHashObj();
    Set& getKeySetObj();
    Zset& getKeyZsetObj();
    int getKeySize();
    int getKeyStringSize();
    int getKeyListSize();
    int getKeyHashSize();
    int getKeySetSize();
    int getKeyZsetSize();
    void rdbLoad(int index);

private:

    String _String;
    List _List;
    Hash _Hash;
    Set _Set;
    Zset _Zset;

    Expire _StringExpire;
    Expire _ListExpire;
    Expire _HashExpire;
    Expire _SetExpire;
    Expire _ZsetExpire;

    LRUCache<string, string> _StringLru;
    LRUCache<string, list<string, __gnu_cxx::__pool_alloc<string>>> _ListLru;
    LRUCache<string, map<string, string, less<string>, __gnu_cxx::__pool_alloc<pair<const string, string>>>> _HashLru;
    LRUCache<string, unordered_set<string, std::hash<string>, std::equal_to<string>, __gnu_cxx::__pool_alloc<string>>> _SetLru; 
    LRUCache<string, SP_Skiplist> _ZsetLru;

    string interceptString(const string& ss, int pos1, int pos2);
    LL getTime();
};

