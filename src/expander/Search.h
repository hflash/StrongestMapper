//
// Created by mingz on 2022/1/7.
//

#ifndef STRONGESTMAPPER_SEARCH_H
#define STRONGESTMAPPER_SEARCH_H

#include "../search/SearchNode.h"
#include "../parser/Environment.h"
#include "DefaultExpander.h"
#include "../queue/DefaultQueue.hpp"

struct SearchResult{
    vector<ActionPath> finalPath;
    vector<int> initialMapping;
    vector<int> finalMapping;
    vector<int> searchNodeNum;
    vector<int> queueNum;
    int patternNum;
};

class Search {
public:
    int searchNum;
    int patternNum;
    int queueNum;
    vector<ActionPath> finalActions;
    vector<int> initialMapping;
    Environment* env;
    Search(Environment* env);
    void SearchPath(int k);
    vector<vector<int>> GoodInitialMapping();
    //search k layer circuits, find the best first layer
    SearchResult SearchKLayer(SearchNode* sn,vector<vector<int>>dagT);
    //search all layer circuits with initial mapping
    SearchResult SearchKLayersWithInitialMapping(vector<int> initialMapping,int k);

};


#endif //STRONGESTMAPPER_SEARCH_H
