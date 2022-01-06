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
public:
    vector<vector<int>> generateDag(vector<int> gateIDs);
    vector<vector<int>> couplingGraph;
    vector<vector<int>> coupling;
    vector<vector<int>> MakeCouplingGraph(vector<vector<int>> couplingList);
    map<int,GateNode> gateInfo;
    int getQubitNum();
    int getGateNum();
    int getDagDepth();
    vector<vector<int>> getGateDag();
    vector<vector<int>> getCouplingGraph();
    vector<int> getTopoGate();

    //Return the parents for a given gate ID
    vector<int> getParentsByID(int gateID);
    //Return the children for a given gate ID
    vector<int> getChildrenByID(int gateID);

    /*
     * For all gates, ignore the gate of IDs in GateIDs,
     * generate new K layer dag from the original gates set
     */
    vector<vector<int>> getNewKLayerDag(vector<int> gateIDs, int K);

    //For a given dag table, return its first layer gate IDs
    vector<int> getFrontLayer(vector<vector<int>> gateDag);
    Environment(string name,vector<vector<int>> coupling);
};


#endif //STRONGESTMAPPER_ENVIRONMENT_H
