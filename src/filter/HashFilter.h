//
// Created by wonder on 2021/12/28.
//

#ifndef STRONGESTMAPPER_HASHFILTER_H
#define STRONGESTMAPPER_HASHFILTER_H

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
 * Here we combine 3 items of nodes:
 * 1. l2pMapping: qubit mapping, array of qubits(int)
 * 2. logicalQubitState: busy cycles of each qubit, int array
 * 3. remainGate: set of
 * */
inline std::size_t hashFunc(SearchNode *node) {

    std::size_t hashResult = 0;
    int numQubits = node->environment->getQubitNum();
    int numRemainGates = node->remainGate.size();
    //conbine into hash: qubit mapping
    for (int x = 0; x < numQubits; x++)
        hash_combine(hashResult, node->l2pMapping[x]);

    //combine into hash: logicalQubitState
    for (int x = 0; x < numQubits; x++) {
        hash_combine(hashResult, node->logicalQubitState[x]);
    }

    //combine into hash: remainGate
    for (int i = 0; i < numRemainGates; i++) {
        hash_combine(hashResult, node->remainGate[i]);
    }

    return hashResult;
}

class HashFilter : public Filter {
private:
    int numFiltered = 0;
    int numMarkedDead = 0;
    std::unordered_map<std::size_t, vector<SearchNode *>> hashmap;

public:
    HashFilter(){

    }
    HashFilter * createEmptyCopy() {
        HashFilter * f = new HashFilter();
        f->numFiltered = this->numFiltered;
        f->numMarkedDead = this->numMarkedDead;
        return f;
    }
    /* for hash filter
     * Consider first the value of the simple hash result, and second the hash conflict,
     * including the following methods:
        * 1. one-to-one comparison of mapping
        * 2. comparison of the number of remaining gates and comparison of gate information (gateID)
        * 3. one-to-one comparison of current qubit states
     * */
    bool filter(SearchNode *newNode) {
        int numQubits = newNode->environment->getQubitNum();
        std::size_t hashResult = hashFunc(newNode);
        for (SearchNode *candidate: this->hashmap[hashResult]) {
            if(candidate->dead){
                continue;
            }
            bool toBeFiltered = true;
            for (int x = 0; x < numQubits; x++) {
                if (candidate->l2pMapping[x] != newNode->l2pMapping[x]) {
                    toBeFiltered = false;
                    break;
                }
            }
            if (newNode->remainGate.size() != candidate->remainGate.size()) {
                toBeFiltered = false;
                break;
            } else if (toBeFiltered) {
                for (int g = 0; g < newNode->remainGate.size(); g++) {
                    if (newNode->remainGate[g] != candidate->remainGate[g]) {
                        toBeFiltered = false;
                        break;
                    }
                }
            }
            bool mapAndGate = toBeFiltered;
            for(int x = 0; mapAndGate && x < numQubits; x++)
            {
                int newNodeBusy = newNode->logicalQubitState[x];
                int timeNew = 0;
                if(newNodeBusy)
                {
                    int cyclePlusBusy = newNodeBusy + newNode->timeStamp;
                    assert(cyclePlusBusy > timeNew);
                    timeNew = cyclePlusBusy;
                }
                int timeCan = 0;
                int candidateBusy = candidate->logicalQubitState[x];
                if(candidateBusy)
                {
                    int cyclePlusBusy = candidateBusy + candidate->timeStamp;
                    assert(cyclePlusBusy > timeCan);
                    timeCan = candidateBusy;
                }
                if(timeNew < timeCan){
                    toBeFiltered = false;
                    mapAndGate = false;
                    //new node is better on this bit
                    //map and remain gates may be different in a few coming steps
                    break;
                }else if(timeNew > timeCan)
                {
                    //new node is worse on this bit
                    //map and remain gates may be different in a few coming steps
                    mapAndGate = false;
                }
            }
            // The time stamp of new node: every qubit in candidate seems
            //      worse or the same, needs to wait for more cycles to idle
            // the time stamp of new node: cycles the same or every qubit worse
            if(toBeFiltered && (newNode->timeStamp == candidate->timeStamp || !mapAndGate)){
                numFiltered++;
                return true;
            }
        }
        this->hashmap[hashResult].push_back(newNode);
        return false;
    }
};

#endif //STRONGESTMAPPER_HASHFILTER_H
