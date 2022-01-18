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
            return moreCx;
        }
    }
    return moreCx;
}

bool DefaultExpander::expand(DefaultQueue *nodes,SearchNode* node) {
    int countNum=0;
    cout<<"father node is :\n";
    node->PrintNode();
    //dead or not
    if(node->dead==true){
        this->expandeNum=this->expandeNum+countNum;
        return false;
    }
    if(this->IsMoreCnot(node)==false){
        // no need for swap
        vector<int> remainGates=node->remainGate;
        vector<int> readGates=node->readyGate;
        vector<ActionPath> scheduledGates=node->actionPath;
        while(readGates.size()>0){
            vector<ScheduledGate> thisTimeSchduledGate;
            // add new schduled gate and generate new remaingates
            for(int i=0;i<readGates.size();i++){
                ScheduledGate Sg;
                Sg.gateName=this->env->gateInfo.find(readGates[i])->second.Name;
                Sg.controlQubit=-1;
                Sg.targetQubit=node->p2lMapping[this->env->gateInfo.find(readGates[i])->second.targetQubit];
                Sg.gateID=readGates[i];
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
            ActionPath thisTimeAction;
            thisTimeAction.actions=thisTimeSchduledGate;
            thisTimeAction.pattern=false;
            scheduledGates.push_back(thisTimeAction);
            countNum++;
        }
        this->findBestNode=true;
        this->expandeNum=this->expandeNum+countNum;
        return true;
    }
    else{
        vector<int> readyGates=node->readyGate;
        int readyGateNum=readyGates.size();
        vector<vector<int>> readyGateCom=this->ComReadyGates(readyGates);
        for(int i=0;i<readyGateCom.size();i++){
            bool IsPattern=true;
            if(readyGateCom[i].size()==readyGateNum){
                IsPattern=false;
            }
            cout<<"this time the ready gates are : ";
            for(int ii=0;ii<readyGateCom[i].size();ii++){
                cout<<readyGateCom[i][ii]<<" ";
            }
            cout<<endl;
            //one of the ready gates combination
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
                Sg.gateID=nodeID;
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
                for (int a = 0; a < nowSwapSize; a++) {
                    set<int> usedQubits;
                    for(int b=0;b<possibleSwapCom[a].size();b++){
                        usedQubits.insert(possibleSwapCom[a][b][0]);
                        usedQubits.insert(possibleSwapCom[a][b][1]);
                    }
                    if(usedQubits.count(possibleSwap[k][0])==0&&usedQubits.count(possibleSwap[k][1])==0){
                        vector<vector<int>> temp1;
                        temp1 = possibleSwapCom[a];
                        temp1.push_back(possibleSwap[k]);
                        possibleSwapCom.push_back(temp1);
                    }
                }
            }
            for(int k=0;k<possibleSwapCom.size();k++){
                cout<<"this time the sawp are : ";
                for(int jj=0;jj<possibleSwapCom[k].size();jj++){
                    cout<<"["<<possibleSwapCom[k][jj][0]<<" "<<possibleSwapCom[k][jj][1]<<"] ";
                }
                cout<<endl;
                vector<vector<int>> temp=possibleSwapCom[k];
                vector<int> newQubitStateAfterReady=newQubitState;
                vector<int> newLogicalMapping=node->l2pMapping;
                vector<ScheduledGate> thisActionPathAfterReady=thisActionPath;
                cout<<"after ready gate this time action size is "<<thisActionPathAfterReady.size()<<endl;
                cout<<"temp size is "<<temp.size()<<endl;
                for(int a=0;a<temp.size();a++){
                    cout<<"there is one swap\n";
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
                    Sg.gateID=-1;
                    thisActionPathAfterReady.push_back(Sg);
                    cout<<"after add one swap this time action size is "<<thisActionPathAfterReady.size()<<endl;
                }
                for(int b=0;b<newQubitStateAfterReady.size();b++){
                    if(newQubitStateAfterReady[b]!=0){
                        newQubitStateAfterReady[b]--;
                    }
                }
                vector<ActionPath> newActionPath=node->actionPath;
                ActionPath thisActionAfterReady;
                thisActionAfterReady.actions=thisActionPathAfterReady;
                thisActionAfterReady.pattern=IsPattern;
                newActionPath.push_back(thisActionAfterReady);
                cout<<"newActionPath size is "<<newActionPath.size()<<endl;
                cout<<"this time action size in newActionPath is "<<newActionPath[newActionPath.size()-1].actions.size()<<endl;
                cout<<"this time action size is "<<thisActionPathAfterReady.size()<<endl;
                if(IsCycle(newActionPath,node->environment->getQubitNum())==false){
                    SearchNode* sn=new SearchNode(newLogicalMapping,newQubitStateAfterReady,newdTable,node->environment,node->timeStamp+1,newActionPath);
                    cout<<"new generate node: --------------------------------------- \n";
                    sn->PrintNode();
                    nodes->push(sn);
                }
                countNum++;
            }
        }
        this->expandeNum=this->expandeNum+countNum;
        return true;
    }

}

bool DefaultExpander::IsCycle(vector<ActionPath> actionPath,int qubitNum) {
    int actionLongth=actionPath.size();
    bool isCycle=false;
    if(actionLongth==0){
        return isCycle;
    }
    ActionPath lastAction=actionPath[actionLongth-1];
    vector<int> qubitSwap(qubitNum,-1);
    int lastLongth=lastAction.actions.size();
    for(int i=0;i<lastAction.actions.size();i++){
        if(lastAction.actions[i].gateName=="swap") {
            isCycle = true;
            qubitSwap[lastAction.actions[i].controlQubit]=lastAction.actions[i].targetQubit;
            qubitSwap[lastAction.actions[i].targetQubit]=lastAction.actions[i].controlQubit;
        }
    }
    cout<<"swap in the last time: ";
    for(int i=0;i<qubitNum;i++){
        cout<<qubitSwap[i]<<" ";
    }
    cout<<endl;
    if (isCycle==false){
        return isCycle;
    }
    else{
        for(int i=actionLongth-1;i>=0;i--){
            for(int j=0;j<actionPath[i].actions.size();j++){
                int a=actionPath[i].actions[j].controlQubit;
                int b=actionPath[i].actions[j].targetQubit;
                if(qubitSwap[a]==b){
                    return true;
                }
                else{
                    if(qubitSwap[a]!=-1){
                        qubitSwap[qubitSwap[a]]=-1;
                        qubitSwap[a]=-1;
                    }
                    if(qubitSwap[b]!=-1){
                        qubitSwap[qubitSwap[b]]=-1;
                        qubitSwap[b]=-1;
                    }
                }
            }
            bool flag=true;
            for(int a=0;a<qubitNum;a++){
                if(qubitSwap[a]!=-1){
                    flag=false;
                }
            }
            if(flag==true){
                return false;
            }
        }
        return false;
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