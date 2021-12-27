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
    vector<vector<int>> gateDag;
    vector<int> topoGate;
    vector<vector<int>> generateDag(vector<int> gateIDs);
public:
    vector<vector<int>> couplingGraph;
    vector<vector<int>> MakeCouplingGraph(vector<vector<int>> couplingList);
    map<int,GateNode> gateInfo;
    int getQubitNum();
    int getGateNum();
    int getDagDepth();
    vector<vector<int>> getCouplingGraph();
    vector<int> getTopoGate();
    vector<int> getParentsByID(int gateID);
    vector<int> getChildrenByID(int gateID);
    vector<vector<int>> getNewKLayerDag(vector<int>, int);
    vector<int> getFrontLayer(vector<vector<int>> gateDag);
    Environment(string name,vector<vector<int>> coupling);

};


#endif //STRONGESTMAPPER_ENVIRONMENT_H
