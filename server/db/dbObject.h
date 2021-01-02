#include "../base/Head.h"

namespace dbObject
{
    //类型
    const int dbString = 0;
    const int dbList = 1;
    const int dbHash = 2;
    const int dbSet = 3;
    const int dbZset = 4;

    //编码
    const int dbEncodingString = 0;
    const int dbEncodingLinkedlist = 1;
    const int dbEncodingRbt = 2;
    const int dbEncodingHt = 3;
    const int dbEncodingSkiplist = 4;

    //RDB默认保存时间--毫秒级
    const long long rdbDefaultTime = 1000;

    const string defaultObjValue = "NULL";
}
