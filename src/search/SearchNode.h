//
// Created by mingz on 2021/12/22.
//

#ifndef STRONGESTMAPPER_SEARCHNODE_H
#define STRONGESTMAPPER_SEARCHNODE_H

#include <vector>
#include <string>
#include <unordered_map>
#include "../parser/Environment.h"

struct ScheduledGate{
    string gateName;
    int targetQubit;
    int controlQubit;
};

class SearchNode {
private:
    void GetReadyGate(Environment* env,vector<vector<int>> dagTable);
    void cost1();

public:
    int qubitNum;
    //initail mapping
    vector<int>* initialMapping;
    //logical on physical
    vector<int> mapping;
    //physical on logical
    vector<int> polMapping;
    //logical qubit state busy time
    vector<int> logQubitState;
    //dagTable
    vector<vector<int>> dagTable;
    //read gate: front layer in dag, logical qubit free, cx: two qubit connect
    vector<int> readyGate;
    //heuristic value and timestamp
    int cost;
    //unscheduled gate
    vector<int> remainGate;
    //action path
    vectot<vector<ScheduledGate>> actionPath;

    SearchNode(vector<int>* initMapping,vector<int>nowMapping,vector<vector<int>> dTable);

};


#endif //STRONGESTMAPPER_SEARCHNODE_H
