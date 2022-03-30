//
// Created by mingz on 2021/12/29.
//

#include "DefaultExpander.h"

int DefaultExpander::nodeCount=1;

DefaultExpander::DefaultExpander(Environment *env) {
    this->env=env;
    this->cycleNum=0;
    this->expandeNum=0;
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
//        <<"count = "<<count<<endl;
        count++;
        vector<ScheduledGate> thisTimeSchduledGate;
        //Each for loop can be executed in one time step.
        for(int i=0;i<readyGate.size();i++){
            ScheduledGate Sg;
            Sg.gateName=this->env->gateInfo.find(readyGate[i])->second.Name;
            if(Sg.gateName=="cx"){
                Sg.controlQubit=node->p2lMapping[this->env->gateInfo.find(readyGate[i])->second.controlQubit];
            }
            else{
                Sg.controlQubit=-1;
            }
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

vector<vector<vector<int>>>
DefaultExpander::SwapCom1(vector<int> qubitState, vector<int> l2pmapping, vector<vector<int>> dTable) {
    //判断这个逻辑比特后续还有没有cnot门的检查
    vector<bool> moreCx(qubitState.size(),false);
    for(int i=0;i<dTable.size();i++){
        for(int j=0;j<dTable[i].size();j++){
            if(dTable[i][j]!=0){
                if(this->env->gateInfo.find(dTable[i][j])->second.controlQubit!=-1){
                    moreCx[i]=true;
                    continue;
                }
            }
        }
    }
//    cout<<"no more cnot :";
//    for(int i=0;i<qubitState.size();i++){
//        cout<<moreCx[i]<<" ";
//    }
//    cout<<endl;
    vector<vector<int>> possibleSwap;
    for(int i=0;i<this->env->coupling.size();i++){
        int a=this->env->coupling[i][0];
        int b=this->env->coupling[i][1];
        if(qubitState[l2pmapping[a]]==0&&qubitState[l2pmapping[b]]==0){
            if(moreCx[l2pmapping[a]]==true||moreCx[l2pmapping[b]]==true){
                possibleSwap.push_back(env->coupling[i]);
            }
        }
    }
//    cout<<"valid swap: ";
//    for(int i=0;i<possibleSwap.size();i++){
//        cout<<possibleSwap[i][0]<<" "<<possibleSwap[i][1]<<"  ";
//    }
//    cout<<endl;
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
    //dead or not 如果这个结点已经死了，那么意味着队列里有比这个结点好的结点都使用过了
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
            /*cout<<"ready gate now:"<<endl;
            for(int j=0;j<readyGateCom[i].size();j++){
                cout<<readyGateCom[i][j]<<" ";
            }
            cout<<endl;*/
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
/*            cout<<"possible swap combine :\n";
            for(int a=0;a<possibleSwap.size();a++){
                for(int b=0;b<possibleSwap[a].size();b++){
                    cout<<"swap: "<<possibleSwap[a][b][0]<<" "<<possibleSwap[a][b][1]<<"  ";
                }
                cout<<endl;
            }*/
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
                        sn->PrintNode();
                        // cout<<"queue test"<<endl;
                        nodes->push(sn);
                        cout<<endl;

                }
            }
        }

    }
    this->findBestNode=false;
    this->expandeNum=countNum;
    return false;
}

bool DefaultExpander::expand1(DefaultQueue *nodes, SearchNode *node) {
    int countNum=0;
    int cycleNum=0;
    //dead or not 如果这个结点已经死了，那么意味着队列里有比这个结点好的结点都使用过了
    if(node->dead==true){
        this->expandeNum=this->expandeNum+countNum;
        return false;
    }
    if(this->IsMoreSwap(node)==false){
        // no need for swap
        this->expandWithoutSwap(node);
        return true;
    }
    else{
        //先执行所有的swap组合，然后再是能执行的ready gate都执行
        vector<vector<vector<int>>> possibleSwap=this->SwapCom1(node->logicalQubitState,node->l2pMapping,node->dagTable);
/*        for(int i=0;i<possibleSwap.size();i++){
            for(int j=0;j<possibleSwap[i].size();j++){
                cout<<"swap: ["<<possibleSwap[i][j][0]<<" "<<possibleSwap[i][j][1]<<"] ";
            }
        }
        cout<<endl;
        */
        for(int i=0; i < possibleSwap.size(); i++){
            //执行所有的swap组合
            vector<int> qubitState1=node->logicalQubitState;
            vector<int> mapping=node->l2pMapping;
            vector<ScheduledGate> thisTimeSchduledGate;
            for(int j=0;j<possibleSwap[i].size(); j++){
                int a=possibleSwap[i][j][0];
                int b=possibleSwap[i][j][1];
                int temp;
                temp=mapping[a];
                mapping[a]=mapping[b];
                mapping[b]=temp;
                qubitState1[mapping[a]]=3;
                qubitState1[mapping[b]]=3;
                ScheduledGate Sg;
                Sg.targetQubit=possibleSwap[i][j][0];
                Sg.controlQubit=possibleSwap[i][j][1];
                Sg.gateName="swap";
                Sg.gateID=-1;
                thisTimeSchduledGate.push_back(Sg);
            }
            //计算p2lmapping 每个逻辑比特上的物理比特
            vector<int> p2lmapping(mapping.size(),-1);
            for(int j=0;j<mapping.size();j++){
                p2lmapping[mapping[j]]=j;
            }
            //执行所有能执行的门，依赖关系是在第一层没有前驱结点，比特要空闲，双比特门要相邻
            vector<int> newReadyGate;
            vector<int> frontLayerGate = this->env->getFrontLayer(node->dagTable);
            for (int j = 0; j < frontLayerGate.size(); j++) {
                int gateid = frontLayerGate[j];
                GateNode nowGate = this->env->gateInfo.find(gateid)->second;
                //single qubit gate
                int ii = p2lmapping[nowGate.targetQubit];
                int jj = p2lmapping[nowGate.controlQubit];
                if (nowGate.controlQubit == -1) {
                    if (qubitState1[nowGate.targetQubit] == 0) {
                        newReadyGate.push_back(gateid);
                    }
                }
                    //two qubits gate
                else if (qubitState1[nowGate.targetQubit] == 0 && qubitState1[nowGate.controlQubit] == 0 &&
                         this->env->couplingGraph[ii][jj] == 1) {
                    newReadyGate.push_back(gateid);
                } else {
                }
            }
            for(int j=0;j<newReadyGate.size();j++){
                //Add the actions performed by this step
                ScheduledGate Sg;
                Sg.targetQubit=node->p2lMapping[this->env->gateInfo.find(newReadyGate[j])->second.targetQubit];
                if(this->env->gateInfo.find(newReadyGate[j])->second.controlQubit!=-1){
                    Sg.controlQubit=node->p2lMapping[this->env->gateInfo.find(newReadyGate[j])->second.controlQubit];
                }
                else{
                    Sg.controlQubit=-1;
                }
                Sg.gateName=this->env->gateInfo.find(newReadyGate[j])->second.Name;
                Sg.gateID=newReadyGate[j];
                thisTimeSchduledGate.push_back(Sg);
                //update qubit state
                qubitState1[this->env->gateInfo.find(newReadyGate[j])->second.targetQubit]=1;
                if(this->env->gateInfo.find(newReadyGate[j])->second.controlQubit!=-1){
                    qubitState1[this->env->gateInfo.find(newReadyGate[j])->second.controlQubit]=1;
                }
            }
            //执行完ready gate后，当前结点的状态
            vector<int> remainGate;
            for(int j=0;j<node->remainGate.size();j++){
                bool flag=true;
                for(int k=0;k<newReadyGate.size();k++){
                    if(newReadyGate[k]==node->remainGate[j]){
                        flag=false;
                        break;
                    }
                }
                if(flag==true){
                    remainGate.push_back(node->remainGate[j]);
                }
            }
            if(remainGate.size()==0){
                ActionPath thisAction;
                thisAction.actions=thisTimeSchduledGate;
                thisAction.pattern=true;
                vector<ActionPath> path=node->actionPath;
                path.push_back(thisAction);
                this->actionPath=path;
                this->expandeNum=countNum;
                return true;
            }
            vector<vector<int>> newDagtable=this->env->generateDag(remainGate);
            //生成新的ActionPath
            ActionPath thisAction;
            thisAction.actions=thisTimeSchduledGate;
            bool IsPattern;
            if(node->readyGate.size()==newReadyGate.size()){
                IsPattern=false;
            }
            else{
                IsPattern=true;
            }
            thisAction.pattern=IsPattern;
            vector<ActionPath> path=node->actionPath;
            path.push_back(thisAction);
            //qubit state update and time
            for(int k=0;k<qubitState1.size();k++){
                if(qubitState1[k]>0){
                    qubitState1[k]--;
                }
            }
            int timeStamp=node->timeStamp+1;
            if(this->IsCycle1(path,qubitState1.size())==false){
                countNum++;
                nodeCount++;
                SearchNode* sn= new SearchNode(mapping,qubitState1,newDagtable,env,timeStamp,path);
                sn->nodeID=nodeCount;
                //sn->PrintNode();

                nodes->push(sn);
                //cout<<endl;
            }
            else{
                cycleNum++;
            }
        }
    }
    this->expandeNum=countNum;
    this->cycleNum=cycleNum;
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

bool DefaultExpander::IsCycle1(vector<ActionPath> actionPath, int qubitNum) {
    //如果isCycle是truen那么有循环，需要被筛除
    bool isCycle=false;
    int actionLongth=actionPath.size();
    //如果路径小于等于3那么不可能有循环，因为一个swap占据了3个时间周期
    if(actionLongth<=3){
        return isCycle;
    }
    //qubitSwap用于记录每个逻辑比特上执行的swap
    //qubitSwap[1]上面记录物理qubit[1]上和哪个物理比特swap了
    vector<int> qubitSwap(qubitNum,-2);
    ActionPath lastAction=actionPath[actionLongth-1];
    for(int i=0;i<lastAction.actions.size();i++){
        if(lastAction.actions[i].gateName=="swap") {
            isCycle = true;
            qubitSwap[lastAction.actions[i].controlQubit]=lastAction.actions[i].targetQubit;
            qubitSwap[lastAction.actions[i].targetQubit]=lastAction.actions[i].controlQubit;
        }
    }
    //当isCycle = true的时候，说明新增加的一轮里面没有swap,那么不可能造成新的cycle
    if(isCycle==false){
        return isCycle;
    }
    else{
        for(int i=actionLongth-2;i>=0;i--){
            for(int j=0;j<actionPath[i].actions.size();j++){
                if(actionPath[i].actions[j].gateName=="swap"){
                    int swap1=actionPath[i].actions[j].controlQubit;
                    int swap2=actionPath[i].actions[j].targetQubit;
                    if(qubitSwap[swap1]==swap2){
                        return true;
                    }
                    else{
                        if(qubitSwap[swap1]!=-2){
                            qubitSwap[qubitSwap[swap1]]=-2;
                            qubitSwap[swap1]=-2;
                        }
                        if(qubitSwap[swap2]!=-2){
                            qubitSwap[qubitSwap[swap2]]=-2;
                            qubitSwap[swap2]=-2;
                        }
                    }
                }
                if(actionPath[i].actions[j].gateName=="cx"){
                    int cx1=actionPath[i].actions[j].controlQubit;
                    int cx2=actionPath[i].actions[j].targetQubit;
                    if(qubitSwap[cx1]!=-2){
                        qubitSwap[qubitSwap[cx1]]=-2;
                        qubitSwap[cx1]=-2;
                    }
                    if(qubitSwap[cx2]!=-2){
                        qubitSwap[qubitSwap[cx2]]=-2;
                        qubitSwap[cx2]=-2;
                    }
                }
            }
            isCycle=false;
            for(int k=0;k<qubitNum;k++){
                if(qubitSwap[k]!=-2){
                    isCycle=true;
                }
            }
            if(isCycle==false){
                return isCycle;
            }
        }
        return false;
    }
}

bool DefaultExpander::IsMoreSwap(SearchNode *node) {
    bool moreSwap=false;
    for(int i=0;i<node->remainGate.size();i++){
        int gateid=node->remainGate[i];
        GateNode nowGate = this->env->gateInfo.find(gateid)->second;
        if(this->env->gateInfo.find(gateid)->second.controlQubit!=-1){
            int ii = node->p2lMapping[nowGate.targetQubit];
            int jj = node->p2lMapping[nowGate.controlQubit];
            if(this->env->couplingGraph[ii][jj] != 1){
                moreSwap=true;
                return moreSwap;
            }
        }
    }
    return moreSwap;
}

bool DefaultExpander::expand2(DefaultQueue *nodes, SearchNode *node, HashFilter_TOQM* filter_T) {
    int countNum=0;
    int cycleNum=0;
    //dead or not 如果这个结点已经死了，那么意味着队列里有比这个结点好的结点都使用过了
    if(node->dead==true){
        this->expandeNum=this->expandeNum+countNum;
        return false;
    }
    if(this->IsMoreSwap(node)==false){
        // no need for swap
        this->expandWithoutSwap(node);
        return true;
    }
    else{
        //先执行所有的swap组合，然后再是能执行的ready gate都执行
        vector<vector<vector<int>>> possibleSwap=this->SwapCom1(node->logicalQubitState,node->l2pMapping,node->dagTable);
        for(int i=0; i < possibleSwap.size(); i++){
            //执行所有的swap组合
            vector<int> qubitState1=node->logicalQubitState;
            vector<int> mapping=node->l2pMapping;
            vector<ScheduledGate> thisTimeSchduledGate;
            for(int j=0;j<possibleSwap[i].size(); j++){
                int a=possibleSwap[i][j][0];
                int b=possibleSwap[i][j][1];
                int temp;
                temp=mapping[a];
                mapping[a]=mapping[b];
                mapping[b]=temp;
                qubitState1[mapping[a]]=3;
                qubitState1[mapping[b]]=3;
                ScheduledGate Sg;
                Sg.targetQubit=possibleSwap[i][j][0];
                Sg.controlQubit=possibleSwap[i][j][1];
                Sg.gateName="swap";
                Sg.gateID=-1;
                thisTimeSchduledGate.push_back(Sg);
            }
            //计算p2lmapping 每个逻辑比特上的物理比特
            vector<int> p2lmapping(mapping.size(),-1);
            for(int j=0;j<mapping.size();j++){
                p2lmapping[mapping[j]]=j;
            }
            //执行所有能执行的门，依赖关系是在第一层没有前驱结点，比特要空闲，双比特门要相邻
            vector<int> newReadyGate;
            vector<int> frontLayerGate = this->env->getFrontLayer(node->dagTable);
            for (int j = 0; j < frontLayerGate.size(); j++) {
                int gateid = frontLayerGate[j];
                GateNode nowGate = this->env->gateInfo.find(gateid)->second;
                //single qubit gate
                int ii = p2lmapping[nowGate.targetQubit];
                int jj = p2lmapping[nowGate.controlQubit];
                if (nowGate.controlQubit == -1) {
                    if (qubitState1[nowGate.targetQubit] == 0) {
                        newReadyGate.push_back(gateid);
                    }
                }
                    //two qubits gate
                else if (qubitState1[nowGate.targetQubit] == 0 && qubitState1[nowGate.controlQubit] == 0 &&
                         this->env->couplingGraph[ii][jj] == 1) {
                    newReadyGate.push_back(gateid);
                } else {
                }
            }
            for(int j=0;j<newReadyGate.size();j++){
                //Add the actions performed by this step
                ScheduledGate Sg;
                Sg.targetQubit=node->p2lMapping[this->env->gateInfo.find(newReadyGate[j])->second.targetQubit];
                if(this->env->gateInfo.find(newReadyGate[j])->second.controlQubit!=-1){
                    Sg.controlQubit=node->p2lMapping[this->env->gateInfo.find(newReadyGate[j])->second.controlQubit];
                }
                else{
                    Sg.controlQubit=-1;
                }
                Sg.gateName=this->env->gateInfo.find(newReadyGate[j])->second.Name;
                Sg.gateID=newReadyGate[j];
                thisTimeSchduledGate.push_back(Sg);
                //update qubit state
                qubitState1[this->env->gateInfo.find(newReadyGate[j])->second.targetQubit]=1;
                if(this->env->gateInfo.find(newReadyGate[j])->second.controlQubit!=-1){
                    qubitState1[this->env->gateInfo.find(newReadyGate[j])->second.controlQubit]=1;
                }
            }
            //执行完ready gate后，当前结点的状态
            vector<int> remainGate;
            for(int j=0;j<node->remainGate.size();j++){
                bool flag=true;
                for(int k=0;k<newReadyGate.size();k++){
                    if(newReadyGate[k]==node->remainGate[j]){
                        flag=false;
                        break;
                    }
                }
                if(flag==true){
                    remainGate.push_back(node->remainGate[j]);
                }
            }
            if(remainGate.size()==0){
                ActionPath thisAction;
                thisAction.actions=thisTimeSchduledGate;
                thisAction.pattern=true;
                vector<ActionPath> path=node->actionPath;
                path.push_back(thisAction);
                this->actionPath=path;
                this->expandeNum=countNum;
                return true;
            }
            vector<vector<int>> newDagtable=this->env->generateDag(remainGate);
            //生成新的ActionPath
            ActionPath thisAction;
            thisAction.actions=thisTimeSchduledGate;
            bool IsPattern;
            if(node->readyGate.size()==newReadyGate.size()){
                IsPattern=false;
            }
            else{
                IsPattern=true;
            }
            thisAction.pattern=IsPattern;
            vector<ActionPath> path=node->actionPath;
            path.push_back(thisAction);
            //qubit state update and time
            for(int k=0;k<qubitState1.size();k++){
                if(qubitState1[k]>0){
                    qubitState1[k]--;
                }
            }
            int timeStamp=node->timeStamp+1;
            if(this->IsCycle1(path,qubitState1.size())==false){
                countNum++;
                SearchNode* sn= new SearchNode(mapping,qubitState1,newDagtable,env,timeStamp,path);
                //sn->PrintNode();
                if(!filter_T->filter(sn)){
                    //cout<<"accept"<<endl;
                    nodeCount++;
                    sn->nodeID=nodeCount;
                    nodes->push(sn);
                }
                else{
                    //cout<<"filter"<<endl;
                }
            }
            else{
                cycleNum++;
            }
        }
    }
    this->expandeNum=countNum;
    this->cycleNum=cycleNum;
    return false;
}

