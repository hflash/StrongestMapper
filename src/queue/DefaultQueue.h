//
// Created by wonder on 2021/12/27.
//

#ifndef STRONGESTMAPPER_DEFAULTQUEUE_H
#define STRONGESTMAPPER_DEFAULTQUEUE_H
#include <iostream>
#include <queue>
#include <vector>
#include "../search/SearchNode.h"
#include "Queue.h"

class DefaultQueue :public Queue{
private:
    struct CmpDefaultQueue
    {
        bool operator()(const SearchNode* lhs, const SearchNode* rhs) const
        {
            return lhs->cost1 < rhs->cost1;
        }
    };
    std::priority_queue<SearchNode*, std::vector<SearchNode*>, CmpDefaultQueue> nodes;
public:
    SearchNode* pop(){
        numPopped++;
        SearchNode* temp = nodes.top();
//        assert(temp->remainGate.size() == 0);
        nodes.pop();
        return temp;
    }
    int size()
    {
        return nodes.size();
    }
};




#endif //STRONGESTMAPPER_DEFAULTQUEUE_H
