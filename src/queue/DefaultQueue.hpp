//
// Created by wonder on 2021/12/27.
//

#ifndef STRONGESTMAPPER_DEFAULTQUEUE_HPP
#define STRONGESTMAPPER_DEFAULTQUEUE_HPP
#include <iostream>
#include <queue>
#include <vector>
#include "../search/SearchNode.h"
#include "Queue.h"
#include "../filter/HashFilter.hpp"
//#include "../filter/Filter.h"

class DefaultQueue :public Queue{
private:
    struct CmpDefaultQueue
    {
        bool operator()(const SearchNode* lhs, const SearchNode* rhs) const
        {
            return lhs->cost1 > rhs->cost1;
        }
    };
    HashFilter* queueFilter = new HashFilter();
    std::priority_queue<SearchNode*, std::vector<SearchNode*>, CmpDefaultQueue> nodes;
    bool pushNode(SearchNode * newNode) {
        nodes.push(newNode);
        return true;
    }

public:
    SearchNode* pop(){
        numPopped++;
        SearchNode* temp = nodes.top();
//        assert(temp->remainGate.size() == 0);
        nodes.pop();
        return temp;
    }
    bool push(SearchNode* newNode) {
        numPushed++;
        if(!this->queueFilter->filter1(newNode)) {
//        if(true) {
//            cout<<"queue +1 true"<<endl;
            bool success = this->pushNode(newNode);
            if(success) {
                return true;
            } else {
                std::cerr << "WARNING: pushNode(Node*) failed somehow.\n";
                return false;
            }
        }
        numFiltered++;
        return false;
    }
    int size()
    {
        return nodes.size();
    }
};




#endif //STRONGESTMAPPER_DEFAULTQUEUE_HPP
