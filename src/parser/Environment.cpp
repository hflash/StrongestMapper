//
// Created by mingz on 2021/12/20.
//

#include "Environment.h"
#include<cassert>
#include<algorithm>

vector<vector<int>> Environment::generateDag(vector<int> gateIDs) {
//    cout<<"begin----------------------------what is gates:\n";
//    for(int i=0;i<gateIDs.size();i++){
//        cout<<gateIDs[i]<<" ";
//    }
//    cout<<endl;
    vector<vector<int>> newDag;
    for (int i = 0; i < this->qubitNum; i++) {
        vector<int> gateOnQubit;
        newDag.push_back(gateOnQubit);
    }
    for(int i=0;i<gateIDs.size();i++) {
        int nowgateid = gateIDs[i];
        int controlQubit = this->gateInfo.find(nowgateid)->second.controlQubit;
        int targetQubit = this->gateInfo.find(nowgateid)->second.targetQubit;
        if (this->gateInfo.find(nowgateid)->second.Name == "cx") {
            int controlQubitTime = newDag[controlQubit].size();
            int targetQubitTime = newDag[targetQubit].size();
            if (controlQubitTime > targetQubitTime) {
                for (int i = 0; i < controlQubitTime - targetQubitTime; i++) {
                    newDag[targetQubit].push_back(0);
                }
            } else {
                for (int i = 0; i < targetQubitTime - controlQubitTime; i++) {
                    newDag[controlQubit].push_back(0);
                }
            }
            newDag[targetQubit].push_back(nowgateid);
            newDag[controlQubit].push_back(nowgateid);
        } else {
            newDag[targetQubit].push_back(nowgateid);
        }
    }
        int nowDagDepth=0;
        for (int i = 0; i < newDag.size(); i++) {
            if (nowDagDepth < newDag[i].size()) {
                nowDagDepth = newDag[i].size();
            }
        }
        for (int i = 0; i < newDag.size(); i++) {
            for (int j = newDag[i].size(); j < nowDagDepth; j++) {
                newDag[i].push_back(0);
            }
        }
        // update the criticality of GateNodes
        for (int i = 0; i < newDag.size(); i++) {
            for (int j = 0; j < nowDagDepth; j++)
                if (newDag[i][j] != 0) {
                    this->gateInfo.find(newDag[i][j])->second.criticality = dagDepth - j;
                }
        }

        for (int i = 0; i < nowDagDepth; i++) {
            for (int j = 0; j < newDag.size(); j++) {
                cout << newDag[j][i] << " ";
            }
            cout << endl;
        }
//    cout<<"begin---------------------\n";
//    for(int i=0;i<newDag.size();i++){
//        cout<<"i="<<i<<" ";
//        for(int j=0;j<newDag[i].size();j++){
//            cout<<newDag[i][j]<<" ";
//        }
//        cout<<endl;
//    }
//    cout<<"end -------------------------\n";
    return newDag;

}

vector<vector<int>> Environment::MakeCouplingGraph(vector<vector<int>> coupling) {
    vector<int> row(qubitNum,999);
    vector<vector<int>> couplingGraph;
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
    return couplingGraph;
}

Environment::Environment(string name, vector<vector<int>> coupling) {
    string filename = name;
    cout << filename << endl;
    QASMparser qasm_parser(filename);
    qasm_parser.GenerateGateInfo();
    this->qubitNum = qasm_parser.GetQubitNum();
    cout << "qubit num:" << this->qubitNum << endl;
    this->gateNum = qasm_parser.GetGateNum();
    this->gateInfo = qasm_parser.GetGateInfo();
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
    vector<int> gateOrder;
    map<int,GateNode>::iterator iter;
    for(iter=this->gateInfo.begin();iter!=this->gateInfo.end();iter++){
        gateOrder.push_back(iter->first);
    }
    this->gateDag=this->generateDag(gateOrder);
    this->dagDepth=this->gateDag[0].size();

//        for(int i=0;i<gateDag.size();i++){
//            cout<<"i="<<i<<" ";
//            for(int j=0;j<gateDag[i].size();j++){
//                cout<<gateDag[i][j]<<" ";
//            }
//            cout<<endl;
//        }
//        cout<<"-------------------------\n";

    //topoGate vector<int>
    for (int i = 0; i < this->dagDepth; i++) {
        set<int> gatesId;
        for (int j = 0; j < this->gateDag.size(); j++) {
            if (this->gateDag[j][i] != 0 && gatesId.find(gateDag[j][i]) == gatesId.end()) {
                gatesId.insert(this->gateDag[j][i]);
                this->topoGate.push_back(this->gateDag[j][i]);
            }
        }
    }
    cout << "topological sort of gates" << endl;
    for (int i = 0; i < topoGate.size(); i++) {
        cout << topoGate[i] << " ";
    }
    cout << endl;
}

int Environment::getQubitNum() {
    return this->qubitNum;
}

int Environment::getGateNum() {
    return this->gateNum;
}

vector<vector<int>> Environment::getCouplingGraph() {
    return this->couplingGraph;
}

int Environment::getDagDepth() {
    return this->dagDepth;
}

vector<int> Environment::getTopoGate() {
    return this->topoGate;
}

vector<vector<int>> Environment::getNewKLayerDag(vector<int> gateIDs, int K)
{
    vector<vector<int>> newKLayerDag;
    for (int i = 0; i < this->qubitNum; i++) {
        vector<int> gateOnQubit;
        newKLayerDag.push_back(gateOnQubit);
    }
    map<int, GateNode>::iterator it;
    for(int i=0;i<gateIDs.size();i++) {
        int nowgateid = gateIDs[i];
        int controlQubit = this->gateInfo.find(nowgateid)->second.controlQubit;
        int targetQubit = this->gateInfo.find(nowgateid)->second.targetQubit;
        if (this->gateInfo.find(nowgateid)->second.Name == "cx") {
            int controlQubitTime = newKLayerDag[controlQubit].size();
            int targetQubitTime = newKLayerDag[targetQubit].size();
            if(controlQubitTime > K && targetQubitTime > K) {
                break;
            }
            if (controlQubitTime > targetQubitTime) {
                for (int i = 0; i < controlQubitTime - targetQubitTime; i++) {
                    newKLayerDag[targetQubit].push_back(0);
                }
            } else {
                for (int i = 0; i < targetQubitTime - controlQubitTime; i++) {
                    newKLayerDag[controlQubit].push_back(0);
                }
            }
            newKLayerDag[targetQubit].push_back(nowgateid);
            newKLayerDag[controlQubit].push_back(nowgateid);
        } else {
            newKLayerDag[targetQubit].push_back(nowgateid);
        }
    }
    int nowDagDepth=0;
    for (int i = 0; i < newKLayerDag.size(); i++) {
        if (nowDagDepth < newKLayerDag[i].size()) {
            nowDagDepth = newKLayerDag[i].size();
        }
    }
    for (int i = 0; i < newKLayerDag.size(); i++) {
        for (int j = newKLayerDag[i].size(); j < nowDagDepth; j++) {
            newKLayerDag[i].push_back(0);
        }
    }
    // update the criticality of GateNodes
    for (int i = 0; i < newKLayerDag.size(); i++) {
        for (int j = 0; j < nowDagDepth; j++)
            if (newKLayerDag[i][j] != 0) {
                this->gateInfo.find(newKLayerDag[i][j])->second.criticality = dagDepth - j;
            }
    }
    for (int i = 0; i < nowDagDepth; i++) {
        for (int j = 0; j < newKLayerDag.size(); j++) {
            cout << newKLayerDag[j][i] << " ";
        }
        cout << endl;
    }

    //get the first K layer(delete the lats element in each qubit)
    for(int i = 0; i < this->qubitNum; i++){
        newKLayerDag[i].pop_back();
    }
    assert(nowDagDepth == K);
    return newKLayerDag;
}

vector<int> Environment::getFrontLayer(vector<vector<int>> gateDag)
{
    vector<int> frontLayer;
    for(int i = 0; i < this->qubitNum; i++){
        frontLayer.push_back(gateDag[i][0]);
    }
    vector<int>::iterator it,it1;
    for(it = frontLayer.begin(); it != frontLayer.end();){
        if(*it == 0){
            frontLayer.erase(it);
            continue;
        }
        it ++;
    }
    sort(frontLayer.begin(),frontLayer.end());
    frontLayer.erase(unique(frontLayer.begin(),frontLayer.end()),frontLayer.end());
    return frontLayer;
}

vector<int> Environment::getParentsByID(int gateID) {
    vector<int> parents;
    int cycles = this->gateInfo.find(gateID)->second.criticality;
    int parentSearchRange = this->dagDepth - cycles;
    int controlQubit = this->gateInfo.find(gateID)->second.controlQubit;
    int targetQubit = this->gateInfo.find(gateID)->second.targetQubit;
    bool target = false;
    bool control = false;
    for (int i = parentSearchRange - 1; i > -1; i--) {
        if(!target && (this->gateDag[targetQubit][i] != 0))
        {
            parents.push_back(this->gateDag[targetQubit][i]);
            target = true;
        }
        if(!control && controlQubit != -1){
            if((this->gateDag[controlQubit][i] != 0)){
                parents.push_back(gateDag[controlQubit][i]);
                control = true;
            }
        }
        else{
            control = true;
        }
    }
    if (parents.size() == 2 && (parents[0] == parents[1])){
        parents.pop_back();
    }
    return parents;
}

vector<int> Environment::getChildrenByID(int gateID){
    vector<int> children;
    int cycles = this->gateInfo.find(gateID)->second.criticality;
    int childrenSearchRange = this->dagDepth - cycles - 1;
    int controlQubit = this->gateInfo.find(gateID)->second.controlQubit;
    int targetQubit = this->gateInfo.find(gateID)->second.targetQubit;
    bool target = false;
    bool control = false;
    for (int i = 0; i < childrenSearchRange; i++) {
        if(!target && (this->gateDag[targetQubit][i+dagDepth-cycles+1] != 0))
        {
            children.push_back(this->gateDag[targetQubit][i+dagDepth-cycles+1]);
            target = true;
        }
        if(!control && controlQubit != -1){
            if((this->gateDag[controlQubit][i+dagDepth-cycles+1] != 0)){
                children.push_back(gateDag[controlQubit][i+dagDepth-cycles+1]);
                control = true;
            }
        }
        else{
            control = true;
        }
    }
    if (children.size() == 2 && (children[0] == children[1])){
        children.pop_back();
    }
    return children;
}


