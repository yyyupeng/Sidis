#pragma once

#include "../base/Head.h"

class skiplistNode;

class skiplistLevel
{
public:
    skiplistLevel() : zforward(nullptr), zspan(0) {}

    skiplistNode *zforward;
    unsigned int zspan;
};

class skiplistNode
{
public:
    skiplistNode(const std::string& obj_, double score_, int level_);

    std::string obj;
    double score;
    skiplistNode *backward;
    unique_ptr<unique_ptr<skiplistLevel>[]> level;
};

class rangespec
{
public:
    rangespec(double min_, double max_) : zmin(min_), zmax(max_), zminex(true), zmaxex(true) {}
    
    double zmin, zmax;
    bool zminex, zmaxex;
};

class Skiplist
{
public:
    Skiplist();
    ~Skiplist();
    skiplistNode *creatNode(const std::string& obj_, double score_, int level_);
    int randomLevel();
    void insertNode(const std::string& obj_, double score_);
    void deleteNode(const std::string& obj_, double score_);
    bool isInRange(rangespec& range);
    skiplistNode *getFirstInRange(rangespec& range);
    skiplistNode *getLastInRange(rangespec& range);
    unsigned long getCountInRange(rangespec& range);
    vector<skiplistNode*> getNodeInRange(rangespec& range);
    /* void displayRange(rangespec& range); */
    void deleteRange(rangespec& range);
    unsigned long getRank(const std::string& obj_, double score_);
    unsigned long getLength() { return len; }

private:
    skiplistNode *head, *tail;
    unsigned long len;
    int level;

    int valueGteMin(double value, rangespec& spec);
    int valueLteMax(double value, rangespec& spec);
};

