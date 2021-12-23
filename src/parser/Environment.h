//
// Created by mingz on 2021/12/20.
//

#ifndef STRONGESTMAPPER_ENVIRONMENT_H
#define STRONGESTMAPPER_ENVIRONMENT_H

#include<vector>
#include<string>
#include<set>
#include "QASMparser.h"

using namespace std;

class Environment {
private:
    int qubitNum;
    int gateNum;
    int dagDepth;
    vector<vector<int>> couplingGraph;
    map<int,GateNode> gateInfo;
    vector<vector<int>> gateDag;
    vector<int> topoGate;
public:
    int getQubitNum();
    int getGateNum();
    int getDagDepth();
    vector<vector<int>> getCouplingGraph();
    map<int,GateNode> getGateInfo();
    vector<int> getTopoGate();
    vector<int> getParentsByID(int gateID);
    vector<int> getChildrenByID(int gateID);
    Environment(string name,vector<vector<int>> coupling);

};


#endif //STRONGESTMAPPER_ENVIRONMENT_H
