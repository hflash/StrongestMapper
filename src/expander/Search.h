//
// Created by mingz on 2022/1/7.
//

#ifndef STRONGESTMAPPER_SEARCH_H
#define STRONGESTMAPPER_SEARCH_H

#include "../search/SearchNode.h"
#include "../parser/Environment.h"
#include "DefaultExpander.h"
#include "../queue/DefaultQueue.hpp"

//search k layer circuits, find the best first layer
//return search node num

class Search {
public:
    int searchNum;
    int patternNum;
    vector<int> initialMapping;
    DefaultQueue NodesQueue;
    Environment* env;

    ActionPath SearchKLayer(SearchNode* sn,vector<vector<int>>dagT);

};


#endif //STRONGESTMAPPER_SEARCH_H
