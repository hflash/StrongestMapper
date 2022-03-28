//
// Created by mingz on 2022/3/22.
//

#ifndef STRONGESTMAPPER_HASHFILTER_TOQM_HPP
#define STRONGESTMAPPER_HASHFILTER_TOQM_HPP

#include "Filter.h"
#include "../search/SearchNode.h"
#include <iostream>
#include <functional>
#include <unordered_map>
#include <vector>

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

inline std::size_t hashFunc2(SearchNode * n) {
    std::size_t hash_result = 0;
    int numQubits = n->qubitNum;

    //combine into hash: qubit map (array of integers)
    for(int x = 0; x < numQubits; x+=4) {
        unsigned int sum = 0;
        for(int y = 0; y < 4 && (x + y) < numQubits; y++) {
            sum = sum << 8;
            sum += 0xff & n->l2pMapping[x + y];
        }
        hash_combine(hash_result, sum);

        hash_combine(hash_result, n->p2lMapping[x]);//added this to try to discourage hash conflict
    }

    //I added the line below because I think it avoids hash conflicts in some benchmarks
    //if(numQubits > 4) hash_combine(hash_result, (int)(n->laq[1]+1)*(int)(n->laq[2]+1)*(int)(n->laq[3]+1)*((int)n->laq[4]+1));

    //adding part of cycle to hash means we filter fewer nodes, but the filter runs much faster:
    hash_combine(hash_result, n->timeStamp >> 3);

    return hash_result;
}

int FindLastExcutedGate(SearchNode* sn,int logicalQubit){
    //找到当前状态sn中，logicalQubit上一个执行了的门，如果没有则返回0
    Environment* env=sn->environment;
    int lastGate=0;
    int firstGate=0;
    //找到接下来在logical qubit上要执行的下一个门，如果接下来没有待执行的门，那么=0
    for(int i=0;i<sn->dagTable[logicalQubit].size();i++){
        if(sn->dagTable[logicalQubit][i]!=0){
            firstGate=sn->dagTable[logicalQubit][i];
            break;
        }
    }
    int i;
    vector<vector<int>> allDagTable=env->getGateDag();
    for(i=allDagTable[logicalQubit].size()-1;i>=0;i--){
        if(allDagTable[logicalQubit][i]==firstGate){
            break;
        }
    }
    for(int j=i-1;j>=0;j--){
        if(allDagTable[logicalQubit][j]!=0){
            lastGate=allDagTable[logicalQubit][j];
            break;
        }
    }
    return lastGate;
}

bool isMin(Environment* env,int a,int b,int x){
    vector<vector<int>> allDagTable=env->getGateDag();
    for(int i=0;i<allDagTable[0].size();i++){
        if(allDagTable[x][i]==a){
            return true;
        }
        if(allDagTable[x][i]==b){
            return false;
        }
    }
}

bool isFather(SearchNode* a,SearchNode* b){
    int apath=a->timeStamp;
    int bpath=b->timeStamp;
    int minPath=0;
    if(apath<bpath){
        minPath=apath;
    }
    else{
        minPath=bpath;
    }
    for(int i=0;i<minPath;i++){
        if(a->actionPath[i].pattern!=b->actionPath[i].pattern){
            return false;
        }
        else{
            if(a->actionPath[i].actions.size()!=b->actionPath[i].actions.size()){
                return false;
            }
            else{
                int pathSize=a->actionPath[i].actions.size();
                for(int j=0;j<pathSize;j++){
                    if(a->actionPath[i].actions[j].gateID!=b->actionPath[i].actions[j].gateID){
                        return false;
                    }
                }
            }
        }
    }
    return true;
}

class HashFilter_TOQM {
private:
    int numFiltered = 0;
    int numMarkedDead = 0;
    bool foundConflict = false;
    std::unordered_map<std::size_t, vector<SearchNode *> > hashmap;

public:
    void deleteRecord(SearchNode *n) {
        std::size_t hash_result = hashFunc2(n);
        vector<SearchNode *> *mapValue = &this->hashmap[hash_result];//Note: I'm terrified of accidentally making an actual copy of the vector here
        //assert(mapValue->size() > 0);
        for (unsigned int blah = 0; blah < mapValue->size(); blah++) {
            SearchNode *n2 = (*mapValue)[blah];
            if (n2 == n) {
                if (mapValue->size() > 1 && blah < mapValue->size() - 1) {
                    std::swap((*mapValue)[blah], (*mapValue)[mapValue->size() - 1]);
                }
                mapValue->pop_back();
                return;
            }
        }
        //assert(false && "hashfilter2 failed to find node to delete");
    }

    bool filter(SearchNode *newNode) {
        //if(newNode->parent && newNode->parent->dead) {
        //	return true;
        //}

        int numQubits = newNode->qubitNum;

        std::size_t hash_result = hashFunc2(newNode);

        int swapCost = 3;
        vector<SearchNode *> *mapValue = &this->hashmap[hash_result];//Note: I'm terrified of accidentally making an actual copy of the vector here
        for (unsigned int blah = mapValue->size() - 1; blah < mapValue->size() && blah >= 0; blah--) {
            SearchNode *candidate = (*mapValue)[blah];

            //if there's a very big gap between nodes' progress then we probably won't benefit from comparing them:
            //if(candidate->cycle - newNode->cycle >= 6 || newNode->cycle - candidate->cycle >= 6) {
            //	continue;
            //}

            if (candidate->dead) {
                continue;
            } //else if(candidate->parent && candidate->parent->dead) {
            //	candidate->dead = true;
            //}

            bool willFilter = candidate->timeStamp <= newNode->timeStamp;
            bool willMarkDead = newNode->timeStamp <= candidate->timeStamp;
            bool canMarkDead = false;

            int cycleDiff = newNode->timeStamp - candidate->timeStamp;
            if (cycleDiff < 0) {
                cycleDiff = -cycleDiff;
            }

            ///*
            //check for simple descendant relationship (without additional gates)
            //需要写如何判断这个两个结点是同一个结点长出来的，具有前驱后继关系
            if (isFather(candidate, newNode)) {
                willFilter = false;
                willMarkDead = false;
            }

            if (willFilter || willMarkDead) {
                if (this->foundConflict || blah == mapValue->size() - 1) {
                    bool conflict = false;
                    for (int x = 0; x < numQubits - 1; x++) {
                        if (candidate->l2pMapping[x] != newNode->l2pMapping[x]) {
                            if (!this->foundConflict) {
                                std::cerr << "//WARNING: hash conflict detected.\n";
                                this->foundConflict = true;
                            }
                            willFilter = false;
                            conflict = true;
                            break;
                        }
                    }
                    if (conflict) {
                        continue;
                    }
                }
            }

            //set willFilter and willMarkDead to false as appropriate based on qubit progress
            //x是逻辑比特
            for (int x = 0; (willFilter || willMarkDead) && x < numQubits; x++) {

                int lastCanGateID = FindLastExcutedGate(candidate, x);
                int lastNewGateID = FindLastExcutedGate(newNode, x);
                int logicalQubit = x;//x在这里是逻辑比特
                int canBusy = 0;
                int newBusy = 0;
                if (logicalQubit >= 0) {
                    canBusy = candidate->logicalQubitState[logicalQubit];
                    newBusy = newNode->logicalQubitState[logicalQubit];
                }
                if (lastNewGateID && !lastCanGateID) {//newNode has scheduled gates that candidate hasn't scheduled
                    //ToDo can maybe avoid setting to false here if candidate
                    // has made more progress on a high-latency swap?
                    willFilter = false;
                    canMarkDead = true;
                    //ToDo can probably be more selective here too:
                    if (newBusy > 1 && candidate->timeStamp + canBusy < newNode->timeStamp + newBusy) {
                        willMarkDead = false;
                    }
                } else if (lastCanGateID && !lastNewGateID) {//candidate has more scheduled gates for this qubit
                    //ToDo can maybe avoid setting to false here if newNode has made more progress on a high-latency swap?
                    willMarkDead = false;
                    //ToDo can probably be more selective here too:
                    if (canBusy > 1 && newNode->timeStamp + newBusy < candidate->timeStamp + canBusy) {
                        willFilter = false;
                    }
                } else if ((lastCanGateID && lastNewGateID) || (!lastCanGateID && !lastNewGateID)) {
                    if (!lastCanGateID || lastCanGateID == lastNewGateID) {//same (un)scheduled gates for this qubit
                        //compare busyness
                        if (logicalQubit >= 0) {
                            if ((willFilter || !canMarkDead) && canBusy > 1) {
                                if (newBusy) {//both nodes are busy
                                    int candidateCycle = canBusy + candidate->timeStamp;
                                    int newCycle = newBusy + newNode->timeStamp;
                                    if (candidateCycle > newCycle) {
                                        willFilter = false;
                                        canMarkDead = true;
                                    }
                                } else {//only candidate is busy
                                    int candidateCycle = canBusy + candidate->timeStamp;
                                    if (candidateCycle > newNode->timeStamp) {
                                        willFilter = false;
                                        canMarkDead = true;
                                    }
                                }
                            }
                            if (willMarkDead && newBusy > 1) {
                                if (canBusy) {//both nodes are busy
                                    int candidateCycle = canBusy + candidate->timeStamp;
                                    int newCycle = newBusy + newNode->timeStamp;
                                    if (newCycle > candidateCycle) {
                                        willMarkDead = false;
                                    }
                                } else {//only newNode is busy
                                    int newCycle = newBusy + newNode->timeStamp;
                                    if (newCycle > candidate->timeStamp) {
                                        willMarkDead = false;
                                    }
                                }
                            }
                        }
                    } else if (isMin(newNode->environment, lastCanGateID, lastNewGateID,
                                     logicalQubit)) {//newNode has scheduled gates candidate hasn't
                        if (willFilter && (newBusy <= 1 || newBusy <= canBusy ||
                                           newNode->timeStamp - candidate->timeStamp >= swapCost)) {
                            willFilter = false;
                            canMarkDead = true;
                        }
                    } else if (isMin(newNode->environment, lastNewGateID, lastCanGateID,
                                     logicalQubit)) {//candidate has more scheduled gates for this qubit
                        if (willMarkDead && (canBusy <= 1 || canBusy <= newBusy ||
                                             candidate->timeStamp - newNode->timeStamp >= swapCost)) {
                            willMarkDead = false;
                        }
                    } else {
                        cout << "error\n";
                    }
                } else {
                    cout << "error\n";
                }
            }

            if (!canMarkDead || willFilter) {
                willMarkDead = false;
            }
            if (willMarkDead) {
                candidate->dead = true;
                numMarkedDead++;
            }

            //remove dead node from vector
            if (candidate->dead) {

                if (mapValue->size() > 1 && blah < mapValue->size() - 1) {
                    std::swap((*mapValue)[blah], (*mapValue)[mapValue->size() - 1]);
                }
                mapValue->pop_back();
            }

            if (willFilter) {
                /*
                //if(newNode->debugVal) {
                    std::cerr << "Filtering node " << newNode->debugID << " (@cycle " << newNode->cycle << ") \n";
                    printNodee(std::cerr, newNode->scheduled);
                    std::cerr << " based on " << candidate->debugID << " (@cycle " << candidate->cycle << ") \n";
                    printNodee(std::cerr, candidate->scheduled);
                    std::cerr << "\n";
                //}
  ;              //*/
                numFiltered++;
                return true;
            }
        }
        mapValue->push_back(newNode);

        return false;
    };

};
#endif //STRONGESTMAPPER_HASHFILTER_TOQM_HPP