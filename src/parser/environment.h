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

class environment {
private:
    int qubitNum;
    int gateNum;
    int dagDepth;
    vector<vector<int>> couplingGraph;
    vector<GateNode> gateInfo;
    vector<vector<int>> gateDag;
    vector<int> topoGate;
public:
    int GetQubitNum();
    int GetGateNum();
    int GetDagDepth();
    vector<vector<int>> GetCouplingGraph();
    vector<GateNode> GetGateInfo();
    vector<int> GetTopoGate();
    //GateNode 补充critical的值,depth-row,删除ID
    //gateInfo map<ID:GateNode>
    //给定ID，找到其前驱后继
    environment(string name,vector<vector<int>> coupling);

};


#endif //STRONGESTMAPPER_ENVIRONMENT_H
