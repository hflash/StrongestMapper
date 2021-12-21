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
    vector<int> row(qubitNum,999);
    for(int i=0;i<qubitNum;i++){
        couplingGraph.push_back(row);
    }
    for (int i=0;i<coupling.size();i++){
        int a=coupling[i][0];
        int b=coupling[i][1];
        couplingGraph[a][b]=1;
        couplingGraph[b][a]=1;
    }
    for (int i=0;i<qubitNum;i++){
        for(int j=0;j<qubitNum;j++){
            for(int k=0;k<qubitNum;k++){
                if(couplingGraph[i][k]+couplingGraph[k][j]<couplingGraph[i][j]){
                    couplingGraph[i][j]=couplingGraph[i][k]+couplingGraph[k][j];
                }
            }
        }
    }
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