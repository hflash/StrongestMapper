//
// Created by mingz on 2022/1/7.
//

#ifndef STRONGESTMAPPER_SEARCH_H
#define STRONGESTMAPPER_SEARCH_H

#include "../search/SearchNode.h"
#include "../parser/Environment.h"
#include "DefaultExpander.h"
#include "../queue/DefaultQueue.hpp"
#include <string>
#include <iostream>
#include <iostream>
#include <fstream>

using namespace std;

struct SearchResult{
    vector<ActionPath> finalPath;
    vector<int> initialMapping;
    vector<int> finalMapping;
    vector<int> searchNodeNum;
    vector<int> queueNum;
    int patternNum;
    int cycleNum;
    int swapNum;
};

class Search {
public:
    Environment* env;
    Search(Environment* env);
    vector<SearchResult*> SearchPath(int k,string type);
    vector<vector<int>> GoodInitialMapping(string type);
    //search k layer circuits, find the best first layer
    //Search the full path of a circuit
    SearchResult SearchKLayer(SearchNode* sn);
    //search all layer circuits with initial mapping
    SearchResult SearchKLayersWithInitialMapping(vector<int> initialMapping,int k);
    vector<vector<int>> SwapSearch(int k);

    //search full path in sn->remainGate
    SearchResult SearchCircuit(SearchNode* sn);
    SearchResult SearchCircuit1(SearchNode* sn);
    //smooth search
    SearchResult SearchSmoothWithInitialMapping(vector<int> mapping,int k);

};


#endif //STRONGESTMAPPER_SEARCH_H
