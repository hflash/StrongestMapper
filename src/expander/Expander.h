//
// Created by mingz on 2021/12/29.
//

#ifndef STRONGESTMAPPER_EXPANDER_H
#define STRONGESTMAPPER_EXPANDER_H

#include "../queue/Queue.h"
#include "../queue/DefaultQueue.hpp"
#include "../search/SearchNode.h"
#include "../filter/Filter.h"
#include "../filter/HashFilter.hpp"
#include "../parser/Environment.h"

class Expander {
public:
    //best node answer
    bool findBestNode;
    int expandeNum;
    int cycleNum;
    vector<ActionPath> actionPath;
    vector<int> initialMapping;

    virtual ~Expander() {};

    //expands given node, unless it has same-or-worse cost than best final node
    //returns false iff given node's cost >= best final node's cost
    virtual bool expand(DefaultQueue *nodes,SearchNode* node) =0;

//    virtual int setArgs(char** argv) {
//        //used to set the expander's parameters via command-line
//        //return number of args consumed
//
//        return 0;
//    }
//
//    virtual int setArgs() {
//        //used to set the expander's parameters via-cin
//        //return number of args consumed
//
//        return 0;
//    }
};
#endif //STRONGESTMAPPER_EXPANDER_H
