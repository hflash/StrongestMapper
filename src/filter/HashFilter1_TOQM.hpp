//
// Created by DELL on 2022/4/2.
//

#ifndef STRONGESTMAPPER_HASHFILTER1_TOQM_HPP
#define STRONGESTMAPPER_HASHFILTER1_TOQM_HPP

#include <iostream>
#include <functional>
#include <unordered_map>
#include <vector>
#include "Filter.h"
#include "../search/SearchNode.h"

#ifndef HASH_COMBINE_FUNCTION
#define HASH_COMBINE_FUNCTION
//based on hash_combine from Boost libraries
template <class T>
inline void hash_combine(std::size_t& seed, const T& v)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
}
#endif

inline std::size_t hashFunc1(SearchNode * n) {
    std::size_t hash_result = 0;
    int numQubits = n->qubitNum;

    //combine into hash: qubit map (array of integers)
    for(int x = 0; x < numQubits; x++) {
        hash_combine(hash_result, n->l2pMapping[x]);
    }

    //combine into hash: ready gate (set of pointers)
    for(auto x = n->readyGate.begin(); x != n->readyGate.end(); x++) {
        hash_combine(hash_result, (std::uintptr_t) (*x));
    }

    return hash_result;
}

class HashFilter1_TOQM {
private:
    int numFiltered = 0;
    std::unordered_map<std::size_t, vector<SearchNode*> > hashmap;

public:

    void deleteRecord(SearchNode * n) {
        std::size_t hash_result = hashFunc1(n);
        vector<SearchNode*> * mapValue = &this->hashmap[hash_result];//Note: I'm terrified of accidentally making an actual copy of the vector here
        for(unsigned int blah = 0; blah <  mapValue->size(); blah++) {
            SearchNode * n2 = (*mapValue)[blah];
            if(n2 == n) {
                if(mapValue->size() > 1 && blah < mapValue->size() - 1) {
                    std::swap((*mapValue)[blah],(*mapValue)[mapValue->size()-1]);
                }
                mapValue->pop_back();
                return;
            }
        }
    }

    bool filter(SearchNode * newNode) {
        int numQubits = newNode->qubitNum;
        std::size_t hash_result = hashFunc1(newNode);

        //auto findNode = this->hashmap.find(hash_result);
        //if(findNode != this->hashmap.end()) {
        for(SearchNode * candidate : this->hashmap[hash_result]) {
            //Node * candidate = findNode->second;
            bool willFilter = true;

            for(int x = 0; x < numQubits; x++) {
                if(candidate->l2pMapping[x] != newNode->l2pMapping[x]) {
                    //std::cerr << "Warning: duplicate hash values.\n";
                    willFilter = false;
                    break;
                }
            }
            if(newNode->readyGate.size() != candidate->readyGate.size()) {
                //std::cerr << "Warning: duplicate hash values.\n";
                willFilter = false;
            } else if(willFilter) {
                for(auto x = newNode->readyGate.begin(), y = candidate->readyGate.begin(); x != newNode->readyGate.end(); x++, y++) {
                    if((*x) != (*y)) {
                        //std::cerr << "Warning: duplicate hash values.\n";
                        willFilter = false;
                        break;
                    }
                }
            }
            bool allEqual = willFilter;
            for(int x = 0; willFilter && x < numQubits; x++) {
                int time = 0;
                int newBusy = newNode->logicalQubitState[x];
                if(newBusy) {
                    int temp = newBusy + newNode->timeStamp;
                    time = temp;
                }
                int time2 = 0;
                int canBusy = candidate->logicalQubitState[x];
                if(canBusy) {
                    int temp = canBusy + candidate->timeStamp;
                    time2 = temp;
                }
                if(time < time2) {
                    //std::cerr << "Warning: duplicate hash values.\n";
                    willFilter = false;
                    allEqual = false;
                    break;
                } else if(time > time2) {
                    allEqual = false;
                }
            }

            if(willFilter && (newNode->timeStamp == candidate->timeStamp || !allEqual)) {
                numFiltered++;
                return true;
            }
        }
        this->hashmap[hash_result].push_back(newNode);

        return false;
    }

    virtual void printStatistics(std::ostream & stream) {
        stream << "//HashFilter filtered " << numFiltered << " total nodes.\n";
    }
};



#endif //STRONGESTMAPPER_HASHFILTER1_TOQM_HPP
