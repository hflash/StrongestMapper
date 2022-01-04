//
// Created by mingz on 2021/12/29.
//

#include "DefaultExpander.h"

DefaultExpander::DefaultExpander(Environment *env) {
    this->env=env;
}

bool DefaultExpander::IsMoreCnot(SearchNode* node) {
    bool moreCx=false;
    for(int i=0;i<node->remainGate.size();i++){
        int gateid=node->remainGate[i];
        if(this->env->gateInfo.find(gateid)->second.controlQubit==-1){
            moreCx=true;
        }
    }
    return moreCx;
}

bool DefaultExpander::expand(Queue *nodes,SearchNode* node) {
    //dead or not
    if(node->dead==true){
        return false;
    }
    if(this->IsMoreCnot(node)){
        // no need for swap
        vector<int> remainGates=node->remainGate;
        vector<int> readGates=node->readyGate;
        vector<vector<ScheduledGate>> scheduledGates=node->actionPath;
        while(readGates.size()>0){
            vector<ScheduledGate> thisTimeSchduledGate;
            // add new schduled gate and generate new remaingates
            for(int i=0;i<readGates.size();i++){
                ScheduledGate Sg;
                Sg.gateName=this->env->gateInfo.find(readGates[i])->second.Name;
                Sg.controlQubit=-1;
                Sg.targetQubit=node->p2lMapping[this->env->gateInfo.find(readGates[i])->second.targetQubit];
                thisTimeSchduledGate.push_back(Sg);
                auto iter = remove(remainGates.begin(),remainGates.end(),readGates[i]);
            }
            //generate new dag table
            vector<vector<int>> newDtable=env->generateDag(remainGates);
            //new qubits state
            vector<int> newQubitState=node->logicalQubitState;
            for(int i=0;i<newQubitState.size();i++){
                if (newQubitState[i]>0){
                    newQubitState[i]--;
                }
            }
            //new ready gates
            readGates=node->GetReadyGate(newDtable,newQubitState);
            scheduledGates.push_back(thisTimeSchduledGate);
        }
        this->findBestNode=true;
        this->actionPath=scheduledGates;
        return true;
    }
    else{
        vector<int> readyGates=node->readyGate;
        vector<vector<int>> readyGateCom=this->ComReadyGates(readyGates);
        for(int i=0;i<readyGateCom.size();i++){
            vector<int> nowGates=readyGateCom[i];
            vector<int> remainGates=node->remainGate;
            vector<int> newQubitState=node->logicalQubitState;
            vector<int> l2pMapping=node->l2pMapping;
            vector<int> p2lMapping=node->p2lMapping;
            vector<ScheduledGate> thisActionPath;
            for(int j=0;j<nowGates.size();j++){
                int nodeID=nowGates[j];
                int controlQubit=this->env->gateInfo.find(nodeID)->second.controlQubit;
                int targetQubit=this->env->gateInfo.find(nodeID)->second.targetQubit;
                if(controlQubit!=-1){
                    newQubitState[controlQubit]=1;
                }
                newQubitState[targetQubit]=1;
                ScheduledGate Sg;
                Sg.targetQubit=node->p2lMapping[this->env->gateInfo.find(nodeID)->second.targetQubit];
                Sg.controlQubit=node->p2lMapping[this->env->gateInfo.find(nodeID)->second.controlQubit];
                Sg.gateName=this->env->gateInfo.find(nodeID)->second.Name;
                thisActionPath.push_back(Sg);
                auto iter = remove(remainGates.begin(),remainGates.end(),nodeID);
            }
            vector<vector<int>> newdTable=this->env->generateDag(remainGates);
            vector<int> freeQubit;
            for(int k=0;k<newQubitState.size();k++){
                if(newQubitState[k]==0){}
                freeQubit.push_back(k);
            }
            vector<vector<int>> possibleSwap;
            for(int k=0;k<this->env->coupling.size();k++){
                int a=this->env->coupling[k][0];
                int b=this->env->coupling[k][1];
                if(newQubitState[l2pMapping[a]]==0&&newQubitState[l2pMapping[b]]==0){
                    possibleSwap.push_back(env->coupling[k]);
                }
            }
            vector<vector<vector<int>>> possibleSwapCom;
            vector<vector<int>> temp;
            possibleSwapCom.push_back(temp);
            for(int k=0;k<possibleSwap.size();k++) {
                int nowSwapSize = possibleSwapCom.size();
                for(int a=0;a<nowSwapSize;a++){
                    vector<vector<int>> temp1;
                    temp1=possibleSwapCom[a];
                    temp1.push_back(possibleSwap[k]);
                    possibleSwapCom.push_back(temp1);
                }
            }
            for(int k=0;k<possibleSwapCom.size();k++){
                vector<vector<int>> temp=possibleSwapCom[k];
                vector<int> newQubitStateAfterReady=newQubitState;
                vector<int> newLogicalMapping=node->l2pMapping;
                vector<ScheduledGate> thisActionPathAfterReady=thisActionPath;
                for(int a=0;a<temp.size();a++){
                    int physicalQubit1=temp[a][0];
                    int physicalQubit2=temp[a][1];
                    int logicalQubit1=l2pMapping[physicalQubit1];
                    int logicalQubit2=l2pMapping[physicalQubit2];
                    newQubitStateAfterReady[logicalQubit1]=3;
                    newQubitStateAfterReady[logicalQubit2]=3;
                    newLogicalMapping[physicalQubit1]=logicalQubit2;
                    newLogicalMapping[physicalQubit2]=logicalQubit1;
                    ScheduledGate Sg;
                    Sg.targetQubit=physicalQubit1;
                    Sg.controlQubit=physicalQubit2;
                    Sg.gateName="swap";
                    thisActionPathAfterReady.push_back(Sg);
                }
                vector<vector<ScheduledGate>> newActionPath=node->actionPath;
                newActionPath.push_back(thisActionPathAfterReady);

                SearchNode* sn=new SearchNode(node->initialMapping,newLogicalMapping,newQubitStateAfterReady,newdTable,node->environment,node->timeStamp+1,newActionPath);
                nodes->push(sn);
            }
        }
    }

}

vector<vector<int>> DefaultExpander::ComReadyGates(vector<int> readyGates) {
    vector<vector<int>> readyGateCom;
    vector<int> temp;
    readyGateCom.push_back(temp);
    for(int i=0;i<readyGates.size();i++){
        int now_size=readyGateCom.size();
        for(int j=0;j<now_size;j++){
            vector<int> temp1=readyGateCom[j];
            temp1.push_back(readyGates[i]);
            readyGateCom.push_back(temp1);
        }

    }
    return readyGateCom;
}