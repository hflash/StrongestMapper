//
// Created by mingz on 2021/12/28.
//

#include "SearchNode.h"

SearchNode::SearchNode(vector<int> nowMapping, vector<int> qubitState, vector<vector<int>> dTable, Environment *env,
                       int nowtime, vector<ActionPath> path) {
    this->l2pMapping = nowMapping;
    this->dead = false;
    this->qubitNum = nowMapping.size();
    this->p2lMapping.resize(this->qubitNum);
    this->dagTable = dTable;
    this->environment = env;
    this->logicalQubitState = qubitState;
    this->timeStamp = nowtime;
    this->p2lMapping.resize(this->qubitNum);
    for (int i = 0; i < qubitNum; i++) {
        this->p2lMapping[this->l2pMapping[i]] = i;
    }
    this->GetReadyGate();
    this->gate2Critiality();
    this->computeCost1();
    this->findRemainGates();
    this->actionPath = path;
}

void SearchNode::findRemainGates() {
    int depth = this->dagTable[0].size();
    for (int i = 0; i < depth; i++) {
        set<int> gatesId;
        for (int j = 0; j < this->dagTable.size(); j++) {
            if (this->dagTable[j][i] != 0 && gatesId.find(this->dagTable[j][i]) == gatesId.end()) {
                gatesId.insert(this->dagTable[j][i]);
                this->remainGate.push_back(this->dagTable[j][i]);
            }
        }
    }
}

void SearchNode::GetReadyGate() {
    map<int, GateNode> gateNodeTable;
    gateNodeTable = this->environment->gateInfo;
    vector<int> frontLayerGate = this->environment->getFrontLayer(this->dagTable);
    for (int i = 0; i < frontLayerGate.size(); i++) {
        int gateid = frontLayerGate[i];
        GateNode nowGate = this->environment->gateInfo.find(gateid)->second;
        //single qubit gate
        int ii = this->p2lMapping[nowGate.targetQubit];
        int jj = this->p2lMapping[nowGate.controlQubit];
        if (nowGate.controlQubit == -1) {
            if (this->logicalQubitState[nowGate.targetQubit] == 0) {
                this->readyGate.push_back(gateid);
            }
        }
            //two qubits gate
        else if (this->logicalQubitState[nowGate.targetQubit] == 0 &&
                 this->logicalQubitState[nowGate.controlQubit] == 0 &&
                 this->environment->couplingGraph[ii][jj] == 1) {
            this->readyGate.push_back(gateid);
        } else {
        }
    }
}

vector<int> SearchNode::GetReadyGate(vector<vector<int>> dTable, vector<int> qubitState) {
    vector<int> frontLayerGate = this->environment->getFrontLayer(dTable);
    vector<int> readyGates;
    for (int i = 0; i < frontLayerGate.size(); i++) {
        int gateid = frontLayerGate[i];
        GateNode nowGate = this->environment->gateInfo.find(gateid)->second;
        //single qubit gate
        int ii = this->p2lMapping[nowGate.targetQubit];
        int jj = this->p2lMapping[nowGate.controlQubit];
        if (nowGate.controlQubit == -1) {
            if (qubitState[nowGate.targetQubit] == 0) {
                readyGates.push_back(gateid);
            }
        }
            //two qubits gate
        else if (qubitState[nowGate.targetQubit] == 0 && qubitState[nowGate.controlQubit] == 0 &&
                 this->environment->couplingGraph[ii][jj] == 1) {
            readyGates.push_back(gateid);
        } else {
        }
    }
    return readyGates;
}

void SearchNode::gate2Critiality() {
    int depth = this->dagTable[0].size();
    for (int i = 0; i < depth; i++) {
        for (int j = 0; j < this->dagTable.size(); j++) {
            int gateid = dagTable[j][i];
            if (this->gateCriticality.find(gateid) == this->gateCriticality.end()) {
                int criticality = depth - i;
                this->gateCriticality.insert({gateid, criticality});
            }
        }
    }
}

int SearchNode::findFreeTimePhysical(int physicalQubit, int gateID) {
    int depth = this->dagTable[0].size();
    int path = 0;
    int i=0;
    int logicalQubit = this->l2pMapping[physicalQubit];
    while (dagTable[logicalQubit][i] != gateID) {
        if(dagTable[logicalQubit][i]==0){
            path++;
        }
        i++;
    }
    path = path + this->logicalQubitState[this->l2pMapping[physicalQubit]];
    return path;
}

int SearchNode::findFreeTimePhysical(int physicalQubit) {
    int depth = this->dagTable[0].size();
    int path = 0;
    int logicalQubit = this->l2pMapping[physicalQubit];
    for (int i = 0; i < depth; i++) {
        int gateid = dagTable[logicalQubit][i];
        if (gateid == 0) {
            path++;
        } else if (this->environment->gateInfo.find(gateid)->second.controlQubit == -1) {
            path++;
        } else {
            break;
        }
    }
    path = path + this->logicalQubitState[this->l2pMapping[physicalQubit]];
    return path;
}

int SearchNode::busyTime() {
    int waitTime = 0;
    for (int i = 0; i < this->qubitNum; i++) {
        if (waitTime < this->logicalQubitState[i]) {
            waitTime = this->logicalQubitState[i];
        }
    }
    return waitTime;
}

vector<int> SearchNode::findFrontTwoQubitsGates() {
    vector<int> frontTwoQubitsGates;
    map<int, int> gateCount;
    int depth = this->dagTable[0].size();
    for (int i = 0; i < this->qubitNum; i++) {
        for (int j = 0; j < depth; j++) {
            if (this->dagTable[i][j] == 0) {
                continue;
            } else {
                if (this->environment->gateInfo.find(this->dagTable[i][j])->second.controlQubit != -1) {
                    if (gateCount.find(this->dagTable[i][j]) == gateCount.end()) {
                        gateCount.insert({this->dagTable[i][j], 1});
                    } else {
                        gateCount.find(this->dagTable[i][j])->second++;
                    }
                    break;
                }
            }
        }
    }
    map<int, int>::iterator miter;
    miter = gateCount.begin();
    while (miter != gateCount.end()) {
        if (miter->second == 2) {
            frontTwoQubitsGates.push_back(miter->first);
        }
        miter++;
    }
    return frontTwoQubitsGates;
}

//vector<int> SearchNode::findFrontTwoQubitsGates() {
//    vector<int> frontTwoQubitsGate;
//    map<int, int> gateCount;
//    int depth = this->dagTable[0].size();
//    for (int i = 0; i < this->qubitNum; i++) {
//        for (int j = 0; j < depth; j++) {
//            if (this->dagTable[i][j] == 0) {
//                continue;
//            } else if (this->environment->gateInfo.find(this->dagTable[i][j])->second.controlQubit == -1) {
//                continue;
//            } else {
//                if (gateCount.find(this->dagTable[i][j]) == gateCount.end()) {
//                    gateCount.insert({this->dagTable[i][j], 1});
//                } else {
//                    gateCount.find(this->dagTable[i][j])->second++;
//                }
//            }
//        }
//    }
//    map<int, int>::iterator miter;
//    miter = gateCount.begin();
//    while (miter != gateCount.end()) {
//        if (miter->second == 2) {
//            frontTwoQubitsGate.push_back(miter->first);
//        }
//        miter++;
//    }
//    return frontTwoQubitsGate;
//}

void SearchNode::computeCost1() {
    int waitTime = busyTime();
    int cost = waitTime;
    vector<int> frontLayer = this->findFrontTwoQubitsGates();
//    cout << "The frontLayer is: \n";
//    for (int i = 0; i < frontLayer.size(); i++) {
//        cout << frontLayer[i] << " ";
//    }
//    cout << endl;
    int gateNum = frontLayer.size();
    map<int, int> gateCost;
    for (int i = 0; i < gateNum; i++) {
        int gateId = frontLayer[i];
//        cout<<"gateId : "<<gateId<<endl;
        int qubit1 = this->environment->gateInfo.find(gateId)->second.controlQubit;
        int qubit2 = this->environment->gateInfo.find(gateId)->second.targetQubit;
//        int length1 = this->findFreeTimePhysical(this->p2lMapping[qubit1]);
        int length1 = this->findFreeTimePhysical(this->p2lMapping[qubit1], gateId);
//        cout << "logical qubit is " << qubit1 << " and the length is " << length1 <<endl;
//        int length2 = this->findFreeTimePhysical(this->p2lMapping[qubit2]);
        int length2 = this->findFreeTimePhysical(this->p2lMapping[qubit2], gateId);
//        cout << "logical qubit is " << qubit2 << " and the length is " << length2 << endl;
        if (length1 < length2) {
            std::swap(length1, length2);
        }
        int totalSwap = this->environment->couplingGraph[this->p2lMapping[qubit1]][this->p2lMapping[qubit2]]-1;
//        cout<<this->p2lMapping[qubit1]<<" "<<this->p2lMapping[qubit2]<<endl;
//        cout<<"total Swap is: "<<totalSwap<<endl;
        int totalSwapCost = totalSwap * 3;
        int slack = length1 - length2;
        int effectiveSlack = (slack / 3) * 3;
        if (effectiveSlack > totalSwapCost) {
            effectiveSlack = totalSwapCost;
        }
        int mutualSwapCost = totalSwapCost - effectiveSlack;
        int extraSwapCost = (0x1 & (mutualSwapCost / 3)) * 3;
        mutualSwapCost -= extraSwapCost;
        mutualSwapCost = mutualSwapCost >> 1;

        int cost1 = 1 +this->gateCriticality.find(gateId)->second + length1 + mutualSwapCost;
        int cost2 = 1 + this->gateCriticality.find(gateId)->second + length2 + mutualSwapCost + effectiveSlack;
//        cout<<"cost1 is "<<cost1<<" "<<"cost2 is "<<cost2<<" critial is "<<this->gateCriticality.find(gateId)->second<<" mutualSwapCost is : "<< mutualSwapCost<<" effectiveSlack is : "<<effectiveSlack<<endl;

        if (cost1 < cost2) {
            cost1 += extraSwapCost;
        } else {
            cost2 += extraSwapCost;
        }
        if (cost1 > cost) {
            cost = cost1;
        }
        if (cost2 > cost) {
            cost = cost2;
        }
    }
    this->cost1 = cost + timeStamp;
}

void SearchNode::PrintNode() {
    cout<<"the mapping now is : ";
    for(int i=0;i<this->l2pMapping.size();i++){
        cout<<this->l2pMapping[i]<<" ";
    }
    cout<<endl;
    cout<<"the cost is : "<<this->cost1<<endl;
    cout<<"the reamin ready gates are : ";
    for(int i=0;i<this->readyGate.size();i++){
        cout<<this->readyGate[i]<<" ";
    }
    cout<<endl;
    cout<<"the qubits state are :";
    for(int i=0;i<this->qubitNum;i++){
        cout<<this->logicalQubitState[i]<<" ";
    }
    cout<<endl;
    cout<<"the action path is : \n";
    for(int i=0;i<this->actionPath.size();i++){
        cout<<"the "<<i<<" step : ";
        for(int j=0;j<actionPath[i].actions.size();j++){
            cout<<actionPath[i].actions[j].gateID<<" "<<actionPath[i].actions[j].gateName<<"     ";
        }
        cout<<endl;
    }
}
