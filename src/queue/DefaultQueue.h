//
// Created by wonder on 2021/12/27.
//

#ifndef STRONGESTMAPPER_DEFAULTQUEUE_H
#define STRONGESTMAPPER_DEFAULTQUEUE_H
#include "../search/SearchNode.h"

class DefaultQueue {
private:
    struct cmpDefaultQueue
    {
        bool operator()(const SearchNode* lhs, const SearchNode* rhs) const
        {

        }
    };
};


#endif //STRONGESTMAPPER_DEFAULTQUEUE_H
