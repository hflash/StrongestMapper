//
// Created by wonder on 2021/12/28.
//

#ifndef STRONGESTMAPPER_HASHFILTER_HPP
#define STRONGESTMAPPER_HASHFILTER_HPP

#include "Filter.h"
#include "../search/SearchNode.h"
#include <iostream>
#include <vector>
#include <unordered_map>
#include <functional>
#include <cassert>

#ifndef HASH_COMBINE_FUNCTION
#define HASH_COMBINE_FUNCTION

////based on hash_combine from Boost libraries
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
inline std::size_t hashFunc2
(SearchNode *node) {

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

class HashFilter : public Filter {
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
            //if newNodeFilter == true then the new node will not be add in the queue
            bool newNodeFilter=true;
            //if odlNodeDead ==true then the old node will marked dead
            bool oldNodeDead=true;
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
                if(newTimeStamp < canTimeStamp){
                    //As long as the new node has a small time on a qubit, it cannot be filtered out
                    newNodeFilter=false;
                }
                if(newTimeStamp > canTimeStamp){
                    oldNodeDead=false;
                }

/*                if (newTimeStamp < canTimeStamp) {
//                    //new node qubit x get the same state earlier
//                    if(markCanDead)
//                        markCanDead = true;
//                    toBeFiltered = false;
//                } else if (newTimeStamp > canTimeStamp) {
//                    markCanDead = false;
//                }
*/
            }
            if(newNodeFilter==true){
                this->numFiltered++;
                return true;
            }
            if(oldNodeDead==true){
                if(candidate->dead==false){
                    candidate->dead=true;
                    this->numMarkedDead++;
                }
            }

/*            if(markCanDead && !toBeFiltered)
//            {
//                this->numMarkedDead++;
//                candidate->dead = true;
//            }else if (toBeFiltered) {
//                this->numFiltered++;
//                return true;
            }
*/
        }
        this->hashmap[hashResult].push_back(newNode);
        return false;
    }

    bool filter1(SearchNode *newNode){
        int numQubits = newNode->environment->getQubitNum();
        std::size_t hashResult = hashFunc2(newNode);
        for (SearchNode *candidate: this->hashmap[hashResult]){
            //candidate如果已经dead则无需考虑
            if (candidate->dead) {
                continue;
            }
            //判断是否是hash冲突
            bool conflict=true;
            //比较mapping
            for(int i=0;i<numQubits;i++){
                if(newNode->l2pMapping[i]!=candidate->l2pMapping[i]){
                    conflict=false;
                }
            }
            //比较remain gate，先比较个数，然后比较大小
            if(candidate->remainGate.size() != newNode->remainGate.size()){
                conflict=false;
            }
            else{
                for(int i=0;i<candidate->remainGate.size();i++){
                    if(candidate->remainGate[i]!=newNode->remainGate[i]){
                        conflict=false;
                    }
                }
            }
            //如果冲突就可以跳过这个candidate了，不冲突才进行大小的比较
            if(conflict==true){
                continue;
            }
            else{

                //新的结点存在比老的结点好的数值，需要留下来
                bool newGood=false;
                //老的结点存在比新的结点好的数值，需要留下来
                bool oldGood=false;
                //判断两个node是否完全相等
                bool equal=true;
                //比较的东西是当前每个比特空闲的时间
                for(int i=0;i<numQubits;i++){
                    int newBusyTime = newNode->logicalQubitState[i];
                    int canBusyTime = candidate->logicalQubitState[i];
                    int newTimeStamp = newBusyTime + newNode->timeStamp;
                    int canTimeStamp = canBusyTime + candidate->timeStamp;
                    if(newTimeStamp<canTimeStamp){
                        newGood=true;
                        equal=false;
                    }
                    if(canTimeStamp<newTimeStamp){
                        oldGood=true;
                        equal=false;
                    }
                }
                if(newGood==false||equal==true){
                    this->numFiltered++;
                    return true;
                }
                if(oldGood==false){
                    candidate->dead=true;
                    this->numMarkedDead++;
                }
            }
        }
        this->hashmap[hashResult].push_back(newNode);
        return false;
    }
};
//template<class T>
//inline void hash_combine(std::size_t &seed, const T &v) {
//    std::hash<T> hasher;
//    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
//}
//
//#endif
//
///*
// * Here we combine 3 items of nodes:
// * 1. l2pMapping: qubit mapping, array of qubits(int)
// * 2. logicalQubitState: busy cycles of each qubit, int array
// * 3. remainGate: set of remaining gates in original circuits
// * */
//inline std::size_t hashFunc(SearchNode *node) {
//
//    std::size_t hashResult = 0;
//    int numQubits = node->environment->getQubitNum();
//    int numRemainGates = node->remainGate.size();
//    //conbine into hash: qubit mapping
//    for (int x = 0; x < numQubits; x++)
//        hash_combine(hashResult, node->l2pMapping[x]);
//
//    //combine into hash: logicalQubitState
//    for (int x = 0; x < numQubits; x++) {
//        hash_combine(hashResult, node->logicalQubitState[x]);
//    }
//
//    //combine into hash: remainGate
//    for (int i = 0; i < numRemainGates; i++) {
//        hash_combine(hashResult, node->remainGate[i]);
//    }
//
//    return hashResult;
//}
//
//class HashFilter : public Filter {
//private:
//    int numFiltered = 0;
//    int numMarkedDead = 0;
//    std::unordered_map<std::size_t, vector<SearchNode *>> hashmap;
//
//public:
//    HashFilter(){
//
//    }
//    HashFilter * createEmptyCopy() {
//        HashFilter * f = new HashFilter();
//        f->numFiltered = this->numFiltered;
//        f->numMarkedDead = this->numMarkedDead;
//        return f;
//    }
//    /* For hash filter
//     * Consider first the value of the simple hash result, and second the hash conflict,
//     * including the following methods:
//        * 1. one-to-one comparison of mapping
//        * 2. comparison of the number of remaining gates and comparison of gate information (gateID)
//        * 3. one-to-one comparison of current qubit states
//     * */
//    bool filter(SearchNode *newNode) {
//        int numQubits = newNode->environment->getQubitNum();
//        std::size_t hashResult = hashFunc(newNode);
//        for (SearchNode *candidate: this->hashmap[hashResult]) {
//            if(candidate->dead){
//                continue;
//            }
//            bool toBeFiltered = true;
//            for (int x = 0; x < numQubits; x++) {
//                if (candidate->l2pMapping[x] != newNode->l2pMapping[x]) {
//                    toBeFiltered = false;
//                    break;
//                }
//            }
//            if (newNode->remainGate.size() != candidate->remainGate.size()) {
//                toBeFiltered = false;
//            } else if (toBeFiltered) {
//                for (int g = 0; g < newNode->remainGate.size(); g++) {
//                    if (newNode->remainGate[g] != candidate->remainGate[g]) {
//                        toBeFiltered = false;
//                        break;
//                    }
//                }
//            }
//            bool mapAndGate = toBeFiltered;
//            for(int x = 0; mapAndGate && x < numQubits; x++)
//            {
//                int newNodeBusy = newNode->logicalQubitState[x];
//                int timeNew = 0;
//                if(newNodeBusy)
//                {
//                    int cyclePlusBusy = newNodeBusy + newNode->timeStamp;
//                    assert(cyclePlusBusy > timeNew);
//                    timeNew = cyclePlusBusy;
//                }
//                int timeCan = 0;
//                int candidateBusy = candidate->logicalQubitState[x];
//                if(candidateBusy)
//                {
//                    int cyclePlusBusy = candidateBusy + candidate->timeStamp;
//                    assert(cyclePlusBusy > timeCan);
//                    timeCan = candidateBusy;
//                }
//                if(timeNew < timeCan){
//                    toBeFiltered = false;
//                    mapAndGate = false;
//                    //new node is better on this bit
//                    //map and remain gates may be different in a few coming steps
//                    break;
//                }else if(timeNew > timeCan)
//                {
//                    //new node is worse on this bit
//                    //map and remain gates may be different in a few coming steps
//                    mapAndGate = false;
//                }
//            }
//            // The time stamp of new node: every qubit in candidate seems
//            //      worse or the same, needs to wait for more cycles to idle
//            // the time stamp of new node: cycles the same or every qubit worse
//            if(toBeFiltered && (newNode->timeStamp == candidate->timeStamp || !mapAndGate)){
//                numFiltered++;
//                return true;
//            }
//        }
//        this->hashmap[hashResult].push_back(newNode);
//        return false;
//    }
//
//};

#endif //STRONGESTMAPPER_HASHFILTER_HPP
