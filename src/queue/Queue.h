//
// Created by wonder on 2021/12/27.
//

#ifndef STRONGESTMAPPER_QUEUE_H
#define STRONGESTMAPPER_QUEUE_H
#include "../search/SearchNode.h"
#include "../parser/Environment.h"
#include "../filter/Filter.h"
#include <iostream>

class Queue {
private:
    //push a node into the priority queue
    //return false iff this fails for any reason
    //pre-condition: our filters have already said this node is good
    //pre-condition: newNode.cost has already been set
    virtual bool pushNode(SearchNode * newNode) = 0;

protected:
    SearchNode * bestFinalNode = 0;
    int numPushed=0,numFiltered=0,numPopped=0;

public:
    virtual ~Queue() {};

    virtual int setArgs(char** argv) {
        //used to set the queue's parameters via command-line
        //return number of args consumed

        return 0;
    }

    virtual int setArgs() {
        //used to set the queue's parameters via std::cin
        //return number of args consumed

        return 0;
    }

    //pop a node and return it
    virtual SearchNode* pop() = 0;

    //return number of elements in queue
    virtual int size() = 0;

    //push a node into the priority queue
    //return false iff this fails for any reason
    //pre-condition: newNode.cost has already been set
    virtual bool push(SearchNode *node) = 0;

    inline SearchNode * getBestFinalNode() {
        return bestFinalNode;
    }
};
#endif //STRONGESTMAPPER_QUEUE_H
