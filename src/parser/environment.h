//
// Created by mingz on 2021/12/20.
//

#ifndef STRONGESTMAPPER_ENVIRONMENT_H
#define STRONGESTMAPPER_ENVIRONMENT_H

#include<vector>
#include<string>
#include "QASMparser.h"

using namespace std;

class environment {
private:
    int qubitNum;
    int gateNum;
    vector<vector<int>> couplingGraph;
    vector<GateNode> gateInfo;
public:
    int GetQubitNum();
    int GetGateNum();
    vector<vector<int>> GetCouplingGraph();
    vector<GateNode> GetGateInfo();
    environment(string name,vector<vector<int>> coupling);

};


#endif //STRONGESTMAPPER_ENVIRONMENT_H
