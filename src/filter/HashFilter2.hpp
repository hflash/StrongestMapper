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

class HashFilter2 : public Filter {
private:
    int numFiltered = 0;
    int numMarkedDead = 0;
    std::unordered_map<std::size_t, vector<SearchNode *>> hashmap;


public:
    /*
     * this filter is designed to assess whether a new node should be push into queue
     * firstly, make sure the result of hashing is conflict or not
     * secondly, whatever conflict, either of the new node or candidate of nodes in queue with the same hash result should be filtered or marked dead
     * * judge conflict:
     * consider if mapping and remain gates are the same
     * * if not hash conflict, judge to filter or not:
     * make sure which node is strictly better by the items followed:
     * 1. cycles + max(busy time)
     * * if hash conflict, continue
     * */
    bool filter(SearchNode *newNode) {
        int numQubits = newNode->environment->getQubitNum();
        std::size_t hashResult = hashFunc2(newNode);
        for (SearchNode *candidate: this->hashmap[hashResult]) {
            if (candidate->dead) {
                continue;
            }
            bool toBeFiltered = true;
            bool hashConflict = false;
            bool markCanDead = true;
            for (int x = 0; x < numQubits; x++) {
                if (candidate->l2pMapping[x] != newNode->l2pMapping[x]) {
                    toBeFiltered = false;
                    hashConflict = true;
                    break;
                }
            }
            if (toBeFiltered && candidate->remainGate.size() == newNode->remainGate.size()) {
                for (int g = 0; g < newNode->remainGate.size(); g++) {
                    if (newNode->remainGate[g] != candidate->remainGate[g]) {
                        hashConflict = true;
                        break;
                    }
                }
            }
            if (hashConflict) {
                continue;
            }
            // compare timeStamp + busytime
            for (int x = 0; x < numQubits; x++) {
                int newBusyTime = newNode->logicalQubitState[x];
                int canBusyTime = candidate->logicalQubitState[x];
                int newTimeStamp = newBusyTime + newNode->timeStamp;
                int canTimeStamp = canBusyTime + candidate->timeStamp;
                if (newTimeStamp < canTimeStamp) {
                    //new node qubit x get the same state earlier
                    if(markCanDead)
                        markCanDead = true;
                    toBeFiltered = false;
                } else if (newTimeStamp > canTimeStamp) {
                    markCanDead = false;
                }
            }
            if(markCanDead && !toBeFiltered)
            {
                candidate->dead = true;
            }else if (toBeFiltered) {
                return true;
            }
        }
        this->hashmap[hashResult].push_back(newNode);
        return false;
    }
};


#endif //STRONGESTMAPPER_HASHFILTER2_HPP
