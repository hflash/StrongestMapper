//
// Created by mingz on 2021/12/28.
//

#ifndef STRONGESTMAPPER_SEARCHNODE_H
#define STRONGESTMAPPER_SEARCHNODE_H


#include <vector>
#include <string>
#include <unordered_map>
#include "../parser/Environment.h"

using namespace std;

struct ScheduledGate{
    string gateName;
    int targetQubit;
    int controlQubit;
    int gateID;
};

struct ActionPath{
    vector<ScheduledGate> actions;
    bool pattern;
};

class SearchNode {
private:
    int pathLength;
    map<int,int> gateCriticality;
    void GetReadyGate();
    void computeCost1();
    void computeCost2();
    void gate2Critiality();
    void findRemainGates();
    int findFreeTimePhysical(int physicalQubit);
    int findFreeTime(int physicalQubit,int gateID);
    int busyTime();
    vector<int> findFrontTwoQubitsGates();

public:
    Environment * environment;
    bool dead;
    int timeStamp;
    int qubitNum;
    int nodeID;
    //initail mapping
    vector<int> initialMapping;
    //logical on physical
    vector<int> l2pMapping;
    //physical on logical
    vector<int> p2lMapping;
    //logical qubit state busy time
    vector<int> logicalQubitState;
    //dagTable
    vector<vector<int>> dagTable;
    //read gate: front layer in dag, logical qubit free, cx: two qubit connect
    vector<int> readyGate;
    //heuristic value and timestamp
    int cost1;
    int cost2;
    //unscheduled gate
    vector<int> remainGate;
    //action path
    vector<ActionPath> actionPath;
    SearchNode(vector<int>nowMapping,vector<int>qubitState,vector<vector<int>> dagTable,Environment *env,int nowtime,vector<ActionPath> path);
    vector<int> GetReadyGate(vector<vector<int>> dTable, vector<int> qubitState);
    void PrintNode();
};


#endif //STRONGESTMAPPER_SEARCHNODE_H
