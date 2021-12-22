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
    int GetQubitNum();
    int GetGateNum();
    int GetDagDepth();
    vector<vector<int>> GetCouplingGraph();
    map<int,GateNode> GetGateInfo();
    vector<int> GetTopoGate();
    //GateNode 补充critical的值,depth-row,删除ID
    //gateInfo map<ID:GateNode>
    //给定ID，找到其前驱后继
    int* getParentsChildrenByID(int gateID);
    Environment(string name,vector<vector<int>> coupling);

};


#endif //STRONGESTMAPPER_ENVIRONMENT_H
