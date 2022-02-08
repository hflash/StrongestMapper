//
// Created by mingz on 2021/12/29.
//

#include "DefaultExpander.h"

DefaultExpander::DefaultExpander(Environment *env) {
    this->env=env;
}

///*delete an element in a vector*/
//void DefaultExpander::DeleteVectorElement(vector<int> V, int element) {
//    int pos=0;
//    for(int i=0;i<V.size();i++){
//        if(V[i]==element){
//            pos=i;
//            break;
//        }
//    }
//    auto iter = V.erase(V.begin() + pos);
//    return;
//
//}

/* True: there is more cnot in an unscheduled
 * False: there is no cnot in an unscheduled, no need for swap
 */
bool DefaultExpander::IsMoreCnot(SearchNode* node) {
    bool moreCx=false;
    for(int i=0;i<node->remainGate.size();i++){
        int gateid=node->remainGate[i];

        if(this->env->gateInfo.find(gateid)->second.controlQubit!=-1){
            moreCx=true;
            return moreCx;
        }
    }
    return moreCx;
}

/* Given the ready gates, give their combination,
 * if not all of them are combined, then the pattern is encountered
 */
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

/*For nodes that do not need swap, execute the path to the end
 *Set findBestNode to true
 *Set the final path to actionPath
 *The number of expands in this step is 1
 */
void DefaultExpander::expandWithoutSwap(SearchNode *node) {
    //All gates that have not yet been executed,
    //only when this vector is empty can the final path be found
    vector<int> remainGate=node->remainGate;
    //All paths currently executed
    vector<ActionPath> path=node->actionPath;
    //some state of the current node
    vector<int> qubitState=node->logicalQubitState;
    vector<int> readyGate=node->readyGate;
    //Execute the current executable gate round by round
    int count=0;
    while(remainGate.size()>0){
//
//        <<"count = "<<count<<endl;
        count++;
        vector<ScheduledGate> thisTimeSchduledGate;
        //Each for loop can be executed in one time step.
        for(int i=0;i<readyGate.size();i++){
            ScheduledGate Sg;
            Sg.gateName=this->env->gateInfo.find(readyGate[i])->second.Name;
            Sg.controlQubit=-1;
            Sg.targetQubit=node->p2lMapping[this->env->gateInfo.find(readyGate[i])->second.targetQubit];
            Sg.gateID=readyGate[i];
            thisTimeSchduledGate.push_back(Sg);
        }
        //Add a new step to the overall path
        ActionPath thisTimeActionPath;
        thisTimeActionPath.actions=thisTimeSchduledGate;
        thisTimeActionPath.pattern=false;
        path.push_back(thisTimeActionPath);
        //node info update
        //qubit state update
        for(int i=0;i<qubitState.size();i++){
            if(qubitState[i]>0){
                qubitState[i]--;
            }
        }
        //remain gates update
        vector<int> newRemainGate;
        for(int i=0;i<remainGate.size();i++){
            bool flag=true;
            for(int j=0;j<readyGate.size();j++){
                if(readyGate[j]==remainGate[i]){
                    flag=false;
                    break;
                }
            }
            if(flag==true){
                newRemainGate.push_back(remainGate[i]);
            }
        }
        remainGate=newRemainGate;
        if(remainGate.size()>0){
            //dagTable update
            vector<vector<int>> newDtable=env->generateDag(remainGate);
            //new ready gates
            readyGate=node->GetReadyGate(newDtable,qubitState);
        }
    }
    this->findBestNode=true;
    this->actionPath=path;
    return;
}

/* Input idle logic bits,
 * Output a combination of swaps that can be executed
 */
vector<vector<vector<int>>> DefaultExpander::SwapCom(vector<int> qubitState,vector<int> l2pmapping) {
    vector<vector<int>> possibleSwap;
    for(int i=0;i<this->env->coupling.size();i++){
        int a=this->env->coupling[i][0];
        int b=this->env->coupling[i][1];
        if(qubitState[l2pmapping[a]]==0&&qubitState[l2pmapping[b]]==0){
            possibleSwap.push_back(env->coupling[i]);
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
    return possibleSwapCom;
}

bool DefaultExpander::expand(DefaultQueue *nodes,SearchNode* node) {
    int countNum=0;
    //dead or not
    if(node->dead==true){
        this->expandeNum=this->expandeNum+countNum;
        return false;
    }
    if(this->IsMoreCnot(node)==false){
        // no need for swap
        this->expandWithoutSwap(node);
        return true;
    }
    else{
        int timeStamp=node->timeStamp+1;
        //Perform one of the Ready Gate combinations
        vector<vector<int>> readyGateCom=this->ComReadyGates(node->readyGate);
        for(int i=0;i<readyGateCom.size();i++){
            //Information that will change after expand in the original node
            vector<int> qubitState1=node->logicalQubitState;
            vector<int> remainGate=node->remainGate;

            bool IsPattern =true;
            if(readyGateCom[i].size()==node->readyGate.size()){
                IsPattern=false;
            }
            vector<ScheduledGate> thisTimeSchduledGate;
            for(int j=0;j<readyGateCom[i].size();j++){
                //Add the actions performed by this step
                ScheduledGate Sg;
                Sg.targetQubit=node->p2lMapping[this->env->gateInfo.find(readyGateCom[i][j])->second.targetQubit];
                if(this->env->gateInfo.find(readyGateCom[i][j])->second.controlQubit!=-1){
                    Sg.controlQubit=node->p2lMapping[this->env->gateInfo.find(readyGateCom[i][j])->second.controlQubit];
                }
                else{
                    Sg.controlQubit=-1;
                }
                Sg.gateName=this->env->gateInfo.find(readyGateCom[i][j])->second.Name;
                Sg.gateID=readyGateCom[i][j];
                thisTimeSchduledGate.push_back(Sg);
                //update qubit state
                qubitState1[this->env->gateInfo.find(readyGateCom[i][j])->second.targetQubit]=1;
                if(this->env->gateInfo.find(readyGateCom[i][j])->second.controlQubit!=-1){
                    qubitState1[this->env->gateInfo.find(readyGateCom[i][j])->second.controlQubit]=1;
                }
            }
            //remain gates update
            vector<int> newRemainGate;
            for(int j=0;j<remainGate.size();j++){
                bool flag=true;
                for(int k=0;k<readyGateCom[i].size();k++){
                    if(readyGateCom[i][k]==remainGate[j]){
                        flag=false;
                        break;
                    }
                }
                if(flag==true){
                    newRemainGate.push_back(remainGate[j]);
                }
            }
            remainGate=newRemainGate;
            if(remainGate.size()==0){
                this->findBestNode=true;
                vector<ActionPath> finalPath;
                finalPath=node->actionPath;
                ActionPath lastAction;
                lastAction.pattern=false;
                lastAction.actions=thisTimeSchduledGate;
                finalPath.push_back(lastAction);
                this->actionPath=finalPath;
                this->expandeNum=countNum;
                return true;
            }
            vector<vector<int>> newdTable=this->env->generateDag(remainGate);
            //possible swap
            vector<vector<vector<int>>> possibleSwap=this->SwapCom(qubitState1,node->l2pMapping);
            for(int j=0;j<possibleSwap.size();j++){
                //node state after ready gate
                vector<int> qubitState2=qubitState1;
                vector<int> mapping=node->l2pMapping;
                vector<ScheduledGate> thisTimeSchduledGate2=thisTimeSchduledGate;
                for(int k=0;k<possibleSwap[j].size();k++){
                    int a=possibleSwap[j][k][0];
                    int b=possibleSwap[j][k][1];
                    int temp;
                    temp=mapping[a];
                    mapping[a]=mapping[b];
                    mapping[b]=temp;
                    qubitState2[mapping[a]]=3;
                    qubitState2[mapping[b]]=3;
                    ScheduledGate Sg;
                    Sg.targetQubit=possibleSwap[j][k][0];
                    Sg.controlQubit=possibleSwap[j][k][1];
                    Sg.gateName="swap";
                    Sg.gateID=-1;
                    thisTimeSchduledGate2.push_back(Sg);
                }
                //qubit state update
                for(int k=0;k<qubitState2.size();k++){
                    if(qubitState2[k]>0){
                        qubitState2[k]--;
                    }
                }
//                cout<<endl;
                ActionPath thisAction;
                thisAction.actions=thisTimeSchduledGate2;
                thisAction.pattern=IsPattern;
                vector<ActionPath> path=node->actionPath;
                path.push_back(thisAction);
                countNum++;
                if(this->IsCycle(path,qubitState2.size())==false){
//                if(true){
                        SearchNode* sn= new SearchNode(mapping,qubitState2,newdTable,env,timeStamp,path);
                       // cout<<"queue test"<<endl;
                        nodes->push(sn);

                }
            }
        }

    }
    this->findBestNode=false;
    this->expandeNum=countNum;
    return false;
}

//Given action path to determine whether there is a loop
bool DefaultExpander::IsCycle(vector<ActionPath> actionPath,int qubitNum) {
    int actionLongth=actionPath.size();
    bool isCycle=false;
    if(actionLongth<5){
        return isCycle;
    }
    ActionPath lastAction=actionPath[actionLongth-1];
    vector<int> qubitSwap(qubitNum,-2);
    for(int i=0;i<lastAction.actions.size();i++){
        if(lastAction.actions[i].gateName=="swap") {
            isCycle = true;
            qubitSwap[lastAction.actions[i].controlQubit]=lastAction.actions[i].targetQubit;
            qubitSwap[lastAction.actions[i].targetQubit]=lastAction.actions[i].controlQubit;
        }
    }
    if (isCycle==false){
        return isCycle;
    }
    else{
            for(int j=0;j<actionPath[actionLongth-4].actions.size();j++){
                int a=actionPath[actionLongth-4].actions[j].controlQubit;
                int b=actionPath[actionLongth-4].actions[j].targetQubit;
                if(actionPath[actionLongth-4].actions[j].gateName=="swap"){
                    if(qubitSwap[b]==a){
                        return true;
                    }
                }
            }
        return false;
    }
}
