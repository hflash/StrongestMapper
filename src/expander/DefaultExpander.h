//
// Created by mingz on 2021/12/29.
//

#ifndef STRONGESTMAPPER_DEFAULTEXPANDER_H
#define STRONGESTMAPPER_DEFAULTEXPANDER_H

#include "Expander.h"
#include <algorithm>

using namespace std;


class DefaultExpander: public Expander {
private:
    bool IsMoreCnot(SearchNode* node);
    vector<vector<int>> ComReadyGates(vector<int> readyGates);
    Environment* env;
public:
    DefaultExpander(Environment* env);
    bool expand( DefaultQueue* nodes,SearchNode* node);
    bool IsCycle(vector<ActionPath> actionPath,int qubitNum);
};


#endif //STRONGESTMAPPER_DEFAULTEXPANDER_H
