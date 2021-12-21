//
// Created by mingz on 2021/12/20.
//

#include "environment.h"

environment::environment(string name,vector<vector<int>>coupling) {
    string filename;
    QASMparser qasm_parser(filename);
    qasm_parser.GenerateGateInfo();
    qubitNum=qasm_parser.GetQubitNum();
    gateNum=qasm_parser.GetGateNum();
    gateInfo=qasm_parser.GetGateInfo();
}

int environment::GetQubitNum() {
    return qubitNum;
}

int environment::GetGateNum() {
    return gateNum;
}

vector<GateNode> environment::GetGateInfo() {
    return gateInfo;
}

vector<vector<int>> environment::GetCouplingGraph() {
    return couplingGraph;
}