//
// Created by wonder on 2022/1/6.
//

#ifndef STRONGESTMAPPER_HASHFILTER2_HPP
#define STRONGESTMAPPER_HASHFILTER2_HPP

#include "Filter.h"
#include "../search/SearchNode.h"
#include <iostream>
#include <vector>
#include <unordered_map>
#include <functional>
#include <cassert>

#ifndef HASH_COMBINE_FUNCTION
#define HASH_COMBINE_FUNCTION

//based on hash_combine from Boost libraries
template<class T>
inline void hash_combine(std::size_t &seed, const T &v) {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

#endif

/*
 * Here we combine 2 items of nodes:
 * 1. l2pMapping: qubit mapping, array of qubits(int)
 * 2. remainGate: set of set of remaining gates in original circuits
 * To make filter more efficient to mark some dead search nodes in queue
 * */
inline std::size_t hashFunc2(SearchNode *node) {

    std::size_t hashResult = 0;
    int numQubits = node->environment->getQubitNum();
    int numRemainGates = node->remainGate.size();
    //conbine into hash: qubit mapping
    for (int x = 0; x < numQubits; x++)
        hash_combine(hashResult, node->l2pMapping[x]);

    //combine into hash: remainGate
    //remainGate is unique topological sort
    for (int i = 0; i < numRemainGates; i++) {
        hash_combine(hashResult, node->remainGate[i]);
    }

    return hashResult;
}

class HashFilter2 : public Filter{
private:
    int numFiltered = 0;
    int numMarkedDead = 0;
    std::unordered_map<std::size_t, vector<SearchNode *>> hashmap;

public:
    bool filter(SearchNode *newNode){
        int numQubits = newNode->environment->getQubitNum();
    }
};


#endif //STRONGESTMAPPER_HASHFILTER2_HPP
