#pragma once

#include "Head.h"
#include <ext/pool_allocator.h>

template <typename T1, typename T2>
struct ListNode
{
    T1 key;
    T2 value;
    ListNode<T1, T2> *pre;
    ListNode<T1, T2> *next;
};

template <typename T1, typename T2>
class LRUCache
{
public:
    LRUCache() : capacity(1000)
    {
        head = new ListNode<T1, T2>;
        tail = new ListNode<T1, T2>;
        head->next = tail;
        tail->pre = head;
    }

    LRUCache(int cap) : capacity(cap)
    {
        head = new ListNode<T1, T2>;
        tail = new ListNode<T1, T2>;
        head->next = tail;
        tail->pre = head;
    }

    ~LRUCache()
    {
        while(head)
        {
            ListNode<T1, T2> *temp = head->next;
            delete head;
            head = temp;
        }
    }

    int get(T1 key)
    {
        auto ret = cache.find(key);
        if(ret != cache.end())
        {
            moveToHead(ret->second);
            return ret->second->value;
        }
        else
            return 0;
    }

    void set(T1 key, T2 value)
    {
        auto ret = cache.find(key);
        if(ret == cache.end())
        {
            ListNode<T1, T2> *newNode(new ListNode<T1, T2>);
            newNode->key = key;
            newNode->value = value;
            cache.insert(make_pair(key, newNode));
            addNode(newNode);
            if(cache.size() > capacity)
            {
                ListNode<T1, T2> *temp = getTail();
                cache.erase(temp->key);
                popTail();
            }
        }
        else
        {
            ret->second->value = value;
            moveToHead(ret->second);
        }
    }

private:
    void addNode(ListNode<T1, T2> *node)
    {
        node->pre = head;
        node->next = head->next;
        head->next->pre = node;
        head->next = node;
    }
   
    void removeNode(ListNode<T1, T2> *node)
    {
        node->pre->next = node->next;
        node->next->pre = node->pre;
        cache.erase(node->key);
        delete node;
    }

    void moveToHead(ListNode<T1, T2> *node)
    {
        if(node)
        {
            node->pre->next = node->next;
            node->next->pre = node->pre;
            addNode(node);
        }
    }

    ListNode<T1, T2> *getTail()
    {
        return tail->pre;
    }

    void popTail()
    {
        removeNode(tail->pre);
    }

private:
    int capacity;
    ListNode<T1, T2> *head, *tail;
    std::unordered_map<T1, ListNode<T1, T2>*, std::hash<T1>, std::equal_to<T1>, __gnu_cxx::__pool_alloc<pair<const T1, ListNode<T1, T2>*>>> cache;
}; 
