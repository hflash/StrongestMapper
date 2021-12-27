//
// Created by mingz on 2021/12/23.
//

#include "SearchNode.h"

SearchNode::SearchNode(vector<int> *initMapping, vector<int> nowMapping, vector<vector<int>> dTable, Environment *env,int nowtime,vectot<vector<ScheduledGate>> path){
    this->mapping = nowMapping;
    this->initialMapping = initMapping;
    this->qubitNum = nowMapping.size();
    this->polMapping.resize(this->qubitNum);
    this->dagTable = dTable;
    this->environment = env;
    this->timeStamp=nowtime;
    for (int i = 0; i < qubitNum; i++) {
        this->polMapping[this->mapping[i]] = i;
    }
    this->readyGate = this->GetReadyGate();
    this->gate2Critiality();
    this->ComputeCost1;
    this->remainGate=this->findRemainGates();
    this->actionPath=path;
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
    map<int, GateNode> *gateNodeTable;
    gateNodeTable = this->environment->gateInfo;
    vector<int> frontLayer;
    vector<int> frontLayerGate = this->environment->getFrontLayer(this->dagTable);
    for (int i = 0; i < frontLayerGate.size(); i++) {
        GateNode nowGate = this->environment->gateInfo.find(frontLayerGate[i])->second;
        if (nowGate.controlQubit == -1 && this->logicalQubitState[nowGate.targetQubit] == 0) {
            frontLayer.push_back(frontLayerGate[i]);
        }
        if (this->logicalQubitState[nowGate.targetQubit] == 0 && this->logicalQubitState[nowGate.controlQubit] == 0 &&
            this->environment->couplingGraph[l2qMapping[nowGate.targetQubit]][l2qMapping[nowGate.controlQubit]] == 1) {
            frontLayer.push_back(frontLayerGate[i]);
        }
    }
    return frontLayer;
}

void SearchNode::gate2Critiality() {
    int depth = this->dagTable[0].size();
    for (int i = 0; i < depth; i++) {
        for (int j = 0; j < this->dagTable.size(); j++) {
            int gateid = dagTable[i][j];
            if (this->gateCriticality.find(gateid) == this->gateCriticality.end()) {
                int criticality = depth - i;
                this->gateCriticality.insert({gateid, criticality});
            }
        }
    }
}

int SearchNode::findFreeTimePhysical(int physicalQubit) {
    int depth = this->dagTable[0].size();
    int path = 0;
    int logicalQubit = this->p2lMapping[physicalQubit];
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
    path=path+this->this->logicalQubitState[this->Mapping[physicalQubit]];
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
    vector<int> frontTwoQubitsGate;
    map<int, int> gateCount;
    int depth = this->dagTable[0].size();
    for (int i = 0; i < this->qubitNum; i++) {
        for (int j = 0; j < depth; j++) {
            if (this->dagTable[i][j] == 0) {
                continue;
            } else if (this->environment->gateInfo.find(this->dagTable[i][j])->second.controlQubit == -1) {
                continue;
            } else {
                if (gateCount.find(this->dagTable[i][j]) == gateCount.end()) {
                    gateCount.insert({this->dagTable[i][j], 1});
                } else {
                    gateCount.find(this->dagTable[i][j])->second++;
                }
            }
        }
    }
    map<int, int>::iterator miter;
    miter = gateCount.begin();
    while (miter != gateCount.end()) {
        if (miter->second == 2) {
            frontTwoQubitsGate.push_back(miter->first);
        }
        miter++;
    }
    return frontTwoQubitsGate;
}

void SearchNode::ComputeCost1() {
    int waitTime = busyTime();
    int cost=waitTime;
    vector<int> frontLayer = this->findFrontTwoQubitsGates();
    int gateNum = frontLayer.size();
    map<int, int> gateCost;
    for (int i = 0; i < gateNum; i++) {
        int gateId = frontLayer[i];
        int qubit1 = this->environment->gateInfo.find(gateId)->second.controlQubit;
        int qubit2 = this->environment->gateInfo.find(gateId)->second.targetQubit;
        int length1 = this->findFreeTimePhysical(this->p2lMapping[qubit1]);
        int length2 = this->findFreeTimePhysical(this->p2lMapping[qubit2]);
        if (length1 < length2) {
            std::swap(length1, length2);
        }
        int totalSwap = this->environment->couplingGraph[this->p2lMapping[qubit1]][this->p2lMapping[qubit2]];
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

        int cost1=1+this->gateCriticality.find(gateId)->second+length1 + mutualSwapCost;
        int cost2 = 1+this->gateCriticality.find(gateId)->second+length2 + mutualSwapCost + effectiveSlack;

        if (cost1 < cost2) {
            cost1 += extraSwapCost;
        } else {
            cost2 += extraSwapCost;
        }
        if(cost1 > cost) {
            cost = cost1;
        }
        if(cost2 > cost) {
            cost = cost2;
        }
    }
    this->cost1=cost+timeStamp;
}
