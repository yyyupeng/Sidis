#include "Skiplist.h"

skiplistNode::skiplistNode(const std::string& obj_, double score_, int level_)
:   obj(obj_),
    score(score_),
    backward(nullptr)
{
    level = unique_ptr<unique_ptr<skiplistLevel>[]>(new unique_ptr<skiplistLevel>[level_]);
    for(int i = 0; i < level_; ++i)
        level[i] = unique_ptr<skiplistLevel>(new skiplistLevel);
}

Skiplist::Skiplist()
:   head(new skiplistNode("", 0, SKIPLIST_MAXLEVEL)),
    tail(nullptr),
    len(0),
    level(1)
{ 
}

Skiplist::~Skiplist()
{
    while(tail)
    {
        skiplistNode *tmp = tail;
        tail = tmp->backward;
        delete tmp;
    }
    delete head;
    head = nullptr;
}

skiplistNode* Skiplist::creatNode(const std::string& obj_, double score_, int level_)
{
    skiplistNode *node = new skiplistNode(obj_, score_, level_);
    return node;
}

int Skiplist::randomLevel()
{
    int level = 1;
    //(rand() & 0xFFFF)只保留低两个字节的位值，其他高位全部清零，所以该值范围为0到0xFFFF
    while ((rand() & 0xFFFF) < (SKIPLIST_P * 0xFFFF))  //level+1的概率为0.25
        level += 1;         //返回一个1到SKIPLIST_MAXLEVEL（32）之间的值
    return (level < SKIPLIST_MAXLEVEL) ? level : SKIPLIST_MAXLEVEL;
}

void Skiplist::insertNode(const std::string& obj_, double score_)
{
    skiplistNode *update[SKIPLIST_MAXLEVEL];
    skiplistNode *x = head;
    unsigned int rank[SKIPLIST_MAXLEVEL];

    //从头结点开始层序遍历(从高往低)
    for(int i = level - 1; i >= 0; --i)
    {
        //更新rank[i]为i+1所跨越的节点数，最外一层为0
        rank[i] = i == (level - 1) ? 0 : rank[i + 1];
        //满足条件则要继续在该层往前遍历
        while(x->level[i]->zforward && (x->level[i]->zforward->score < score_ || 
                (x->level[i]->zforward->score == score_ && x->level[i]->zforward->obj != obj_)))
        {
            //记录该层一共跨越了多少节点 + 上一层遍历所跨越的节点数
            rank[i] += x->level[i]->zspan;
            x = x->level[i]->zforward;
        }
        //记录第i层所遍历到的最后一个节点，遍历到i=0时，就要在该节点后要插入节点
        update[i] = x;
    }

    //获得一个随机层数，将大于等于原level层以上的rank[], update[]更新
    int zlevel = randomLevel();
    if(zlevel > level)
    {
        for(int i = level; i < zlevel; ++i)
        {
            rank[i] = 0;
            update[i] = head;
            update[i]->level[i]->zspan = len;
        }
        level = zlevel;     //更新level
    }

    //插入新节点
    x = creatNode(obj_, score_, level);
    for(int i = 0; i < zlevel; ++i)
    {
        x->level[i]->zforward = update[i]->level[i]->zforward;
        update[i]->level[i]->zforward = x;
        x->level[i]->zspan = update[i]->level[i]->zspan - (rank[0] - rank[i]);
        update[i]->level[i]->zspan = (rank[0] - rank[i]) + 1;
    }
    //如果插入节点的level小于原来的level才会执行
    //因为高度没有达到这些层，所以只需将查找时每层最后一个节点的值的跨度加1
    for(int i = zlevel; i < level; ++i)
        update[i]->level[i]->zspan++;

    //设置插入节点的后退指针，就是查找时最下层的最后一个节点，该节点的地址记录在update[0]中
    //如果插入在第二个节点，也就是头结点后的位置就将后退指针设置为NULL
    x->backward = (update[0] == head) ? nullptr : update[0];

    if(x->level[0]->zforward)
        x->level[0]->zforward->backward = x;
    else
        tail = x;
    len++;
}

void Skiplist::deleteNode(const std::string& obj_, double score_)
{
    skiplistNode *update[SKIPLIST_MAXLEVEL];
    skiplistNode *x = head;

    for(int i = level - 1; i >= 0; --i)
    {
        while(x->level[i]->zforward && (x->level[i]->zforward->score < score_ ||
                (x->level[i]->zforward->score == score_ && x->level[i]->zforward->obj != obj_)))
        {
            x = x->level[i]->zforward;
        }
        update[i] = x;
    }

    x = x->level[0]->zforward;
    //找到要被删除节点
    if(x && x->score == score_ && x->obj == obj_)
    {
        //设置前进指针和跨度
        for(int i = 0; i < level; ++i)
        {
            if(update[i]->level[i]->zforward == x)
            {
                update[i]->level[i]->zspan += x->level[i]->zspan - 1;
                update[i]->level[i]->zforward = x->level[i]->zforward;
            }
            else
                update[i]->level[i]->zspan -= 1;
        }
        //设置后退指针
        if(x->level[0]->zforward)
            x->level[0]->zforward->backward = x->backward;
        else
            tail = x->backward;

        while(level > 1 && head->level[level - 1]->zforward == nullptr)
            level--;
        len--;
    }
    delete x;
}

bool Skiplist::isInRange(rangespec& range)
{
    skiplistNode *tmp;
    //排除错误范围
    if(range.zmin > range.zmax || (range.zmin == range.zmax && (range.zminex || range.zmaxex)))
        return false;

    tmp = tail;     //max
    if(tmp == nullptr || !valueGteMin(tmp->score, range))
        return false;
    tmp = head->level[0]->zforward;     //min
    if(tmp == nullptr || !valueLteMax(tmp->score, range))
        return false;

    return true;
}

skiplistNode* Skiplist::getFirstInRange(rangespec& range)
{
    skiplistNode *tmp;
    if(!isInRange(range))
        return nullptr;

    tmp = head;
    for(int i = level - 1; i >= 0; --i)
    {
        while(tmp->level[i]->zforward && !valueGteMin(tmp->level[i]->zforward->score, range))
            tmp = tmp->level[i]->zforward;
    }
    tmp = tmp->level[0]->zforward;  //目标节点
    if(!valueLteMax(tmp->score, range))
        return nullptr;

    return tmp;
}

skiplistNode* Skiplist::getLastInRange(rangespec& range)
{
    skiplistNode *tmp;
    if(!isInRange(range))
        return nullptr;

    tmp = head;
    for(int i = level - 1; i >= 0; --i)
    {
        while(tmp->level[i]->zforward && !valueLteMax(tmp->level[i]->zforward->score, range))
            tmp = tmp->level[i]->zforward;
    }
    if(!valueGteMin(tmp->score, range))
        return nullptr;

    return tmp;
}

unsigned long Skiplist::getCountInRange(rangespec& range)
{
    skiplistNode *tmp = head;
    unsigned long count = 0;

    for(int i = level - 1; i >= 0; --i)
    {
        while(tmp->level[i]->zforward && (range.zminex ?
                tmp->level[i]->zforward->score <= range.zmin :
                tmp->level[i]->zforward->score < range.zmin))
            tmp = tmp->level[i]->zforward;
    }
    tmp = tmp->level[0]->zforward;

    while(tmp && (range.zmaxex ? tmp->score < range.zmax : tmp->score <= range.zmax))
    {
        count++;
        tmp = tmp->level[0]->zforward;
    }
    return count;
}

vector<skiplistNode*> Skiplist::getNodeInRange(rangespec& range)
{
    skiplistNode *tmp = head;
    vector<skiplistNode*> ret;

    for(int i = level - 1; i >= 0; --i)
    {
        while(tmp->level[i]->zforward && (range.zminex ?
                tmp->level[i]->zforward->score <= range.zmin :
                tmp->level[i]->zforward->score < range.zmin))
            tmp = tmp->level[i]->zforward;
    }
    tmp = tmp->level[0]->zforward;

    while(tmp && (range.zmaxex ? tmp->score < range.zmax : tmp->score <= range.zmax))
    {
        ret.emplace_back(tmp);
        tmp = tmp->level[0]->zforward;
    }
    return ret;
}

/*
void Skiplist::displayRange(rangespec& range)
{
    skiplistNode *tmp = head;

    for(int i = level - 1; i >= 0; --i)
    {
        while(tmp->level[i]->zforward && (range.zminex ?
                tmp->level[i]->zforward->score <= range.zmin :
                tmp->level[i]->zforward->score < range.zmin))
            tmp = tmp->level[i]->zforward;
    }
    tmp = tmp->level[0]->zforward;

    while(tmp && (range.zmaxex ? tmp->score < range.zmax : tmp->score <= range.zmax))
    {
        std::cout << tmp->obj << " : " << tmp->score << std::endl;
        tmp = tmp->level[0]->zforward;
    }
}*/

void Skiplist::deleteRange(rangespec& range)
{
    skiplistNode *update[SKIPLIST_MAXLEVEL];
    skiplistNode *tmp = head;

    for(int i = level - 1; i >= 0; --i)
    {
        while(tmp->level[i]->zforward && (range.zminex ?
                tmp->level[i]->zforward->score <= range.zmin :
                tmp->level[i]->zforward->score < range.zmin))
            tmp = tmp->level[i]->zforward;
        update[i] = tmp;
    }
    tmp = tmp->level[0]->zforward;

    while(tmp && (range.zmaxex ? tmp->score < range.zmax : tmp->score <= range.zmax))
    {
        skiplistNode *temp = tmp->level[0]->zforward;
        //设置前进指针和跨度
        for(int i = 0; i < level; ++i)
        {
            if(update[i]->level[i]->zforward == tmp)
            {
                update[i]->level[i]->zspan += tmp->level[i]->zspan - 1;
                update[i]->level[i]->zforward = tmp->level[i]->zforward;
            }
            else
                update[i]->level[i]->zspan -= 1;
        }
        //设置后退指针
        if(tmp->level[0]->zforward)
            tmp->level[0]->zforward->backward = tmp->backward;
        else
            tail = tmp->backward;

        while(level > 1 && head->level[level - 1]->zforward == nullptr)
            level--;
        len--;

        delete tmp;
        tmp = temp;
    }
}

unsigned long Skiplist::getRank(const std::string& obj_, double score_)
{
    skiplistNode *tmp = head;
    unsigned long rank = 0;
    for(int i = level - 1; i >= 0; ++i)
    {
        while(tmp->level[i]->zforward && (tmp->level[i]->zforward->score < score_ ||
                (tmp->level[i]->zforward->score == score_ && tmp->level[i]->zforward->obj != obj_)))
        {
            rank += tmp->level[i]->zspan;
            tmp = tmp->level[i]->zforward;
        }
        if(tmp && tmp->obj == obj_)
            return rank;
    }
    return 0;
}

//value 是否大于（或大于等于）范围 spec 中的 zmin 
//如果返回1表示value大于等于zmin，否则返回0
int Skiplist::valueGteMin(double value, rangespec& spec)
{
    return spec.zminex ? (value > spec.zmin) : (value >= spec.zmin);
}

//value是否小于或小于等于范围 spec 中的 zmax
//返回1表示value小于等于zmax，否则返回0
int Skiplist::valueLteMax(double value, rangespec& spec)
{
    return spec.zmaxex ? (value < spec.zmax) : (value <= spec.zmax);
}



