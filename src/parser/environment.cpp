//
// Created by mingz on 2021/12/20.
//

#include "environment.h"

environment::environment(string name,vector<vector<int>>coupling) {
    string filename=name;
    cout<<filename<<endl;
    QASMparser qasm_parser(filename);
    qasm_parser.GenerateGateInfo();
    this->qubitNum=qasm_parser.GetQubitNum();
    cout<<"qubit num:"<<this->qubitNum<<endl;
    this->gateNum=qasm_parser.GetGateNum();
    this->gateInfo=qasm_parser.GetGateInfo();
    //coupling graph
//    vector<int> row(qubitNum,999);
//    for(int i=0;i<qubitNum;i++){
//        couplingGraph.push_back(row);
//    }
//    for (int i=0;i<coupling.size();i++){
//        int a=coupling[i][0];
//        int b=coupling[i][1];
//        couplingGraph[a][b]=1;
//        couplingGraph[b][a]=1;
//    }
//    for (int i=0;i<qubitNum;i++){
//        for(int j=0;j<qubitNum;j++){
//            for(int k=0;k<qubitNum;k++){
//                if(couplingGraph[i][k]+couplingGraph[k][j]<couplingGraph[i][j]){
//                    couplingGraph[i][j]=couplingGraph[i][k]+couplingGraph[k][j];
//                }
//            }
//        }
//    }
    //gate dag
    for (int i=0;i<this->qubitNum;i++){
        vector<int> gateOnQubit;
        gateDag.push_back(gateOnQubit);
    }
    for (int i=0;i<this->gateInfo.size();i++){
        int nowgateid=gateInfo[i].gateId;
        int controlQubit=this->gateInfo[i].controlQubit;
        int targetQubit=this->gateInfo[i].targetQubit;
        if (this->gateInfo[i].Name=="cx"){
            int controlQubitTime=this->gateDag[controlQubit].size();
            int targetQubitTime=this->gateDag[targetQubit].size();
            if (controlQubitTime>targetQubitTime){
                for(int i=0;i<controlQubitTime-targetQubitTime;i++){
                    this->gateDag[targetQubit].push_back(0);
                }
            }
            else{
                for(int i=0;i<targetQubitTime-controlQubitTime;i++){
                    this->gateDag[controlQubit].push_back(0);
                }
            }
            this->gateDag[targetQubit].push_back(gateInfo[i].gateId);
            this->gateDag[controlQubit].push_back(gateInfo[i].gateId);
        }
        else{
            this->gateDag[targetQubit].push_back(gateInfo[i].gateId);
        }
//        for(int i=0;i<gateDag.size();i++){
//            cout<<"i="<<i<<" ";
//            for(int j=0;j<gateDag[i].size();j++){
//                cout<<gateDag[i][j]<<" ";
//            }
//            cout<<endl;
//        }
//        cout<<"-------------------------\n";

    }
    this->dagDepth=0;
    for(int i=0;i<this->gateDag.size();i++){
        if(this->dagDepth<this->gateDag[i].size()){
            this->dagDepth=this->gateDag[i].size();
        }
    }
    for(int i=0;i<this->gateDag.size();i++){
        for(int j=this->gateDag[i].size();j<this->dagDepth;j++){
            this->gateDag[i].push_back(0);
        }
    }
    for(int i=0;i<this->dagDepth;i++){
        for(int j=0;j<this->gateDag.size();j++){
            cout<<this->gateDag[j][i]<<" ";
        }
        cout<<endl;
    }
    //topoGate vector<int>
    for(int i=0;i<this->dagDepth;i++){
        set<int> gatesId;
        for(int j=0;j<this->gateDag.size();j++){
            if(this->gateDag[j][i]!=0&&gatesId.find(gateDag[j][i])==gatesId.end()){
                gatesId.insert(this->gateDag[j][i]);
                this->topoGate.push_back(this->gateDag[j][i]);
            }
        }
    }
    for(int i=0;i<topoGate.size();i++){
        cout<<topoGate[i]<<" ";
    }
    cout<<endl;
}

int environment::GetQubitNum() {
    return this->qubitNum;
}

int environment::GetGateNum() {
    return this->gateNum;
}

vector<GateNode> environment::GetGateInfo() {
    return this->gateInfo;
}

vector<vector<int>> environment::GetCouplingGraph() {
    return this->couplingGraph;
}

int environment::GetDagDepth() {
    return this->dagDepth;
}

vector<int> environment::GetTopoGate() {
    return this->topoGate;
}