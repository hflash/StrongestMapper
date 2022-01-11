//
// Created by mingz on 2022/1/7.
//

#ifndef STRONGESTMAPPER_SEARCH_H
#define STRONGESTMAPPER_SEARCH_H

#include "../search/SearchNode.h"
#include "../parser/Environment.h"
#include "DefaultExpander.h"
#include "../queue/DefaultQueue.hpp"

class Search {
public:
    int searchNum;
    int patternNum;
    int queueNum;
    vector<int> initialMapping;
    Environment* env;
    Search(Environment* env);
    //search k layer circuits, find the best first layer
    vector<ActionPath> SearchKLayer(vector<int>initialMapping,vector<int>qubitState,vector<vector<int>>dagT);
    void SearchPath(int k);
    vector<vector<int>> GoodInitialMapping();

};


#endif //STRONGESTMAPPER_SEARCH_H
