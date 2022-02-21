//
// Created by mingz on 2022/1/7.
//

#include "Search.h"

Search::Search(Environment *env) {
    this->env = env;
}

vector<vector<int>> Search::SwapSearch(int klayer) {
    vector<vector<int>> possibleSwap = this->env->coupling;
    vector<vector<vector<int>>> possibleSwapCom;
    vector<vector<int>> temp;
    possibleSwapCom.push_back(temp);
    for (int k = 0; k < possibleSwap.size(); k++) {
        cout<<"new swap:"<<possibleSwap[k][0]<<" "<<possibleSwap[k][1]<<endl;
        int nowSwapSize = possibleSwapCom.size();
        for (int a = 0; a < nowSwapSize; a++) {
            set<int> usedQubits;
            for(int b=0;b<possibleSwapCom[a].size();b++){
                int qubit1=possibleSwapCom[a][b][0];
                int qubit2=possibleSwapCom[a][b][1];
                usedQubits.insert(qubit1);
                usedQubits.insert(qubit2);
            }
            //cout<<"set size: "<<usedQubits.size()<<endl;
            if(usedQubits.count(possibleSwap[k][0])==0&&usedQubits.count(possibleSwap[k][1])==0){
                //cout<<"possibleSwap K:"<<possibleSwap[k][0]<<" "<<possibleSwap[k][1]<<endl;
                vector<vector<int>> temp1;
                temp1 = possibleSwapCom[a];
                temp1.push_back(possibleSwap[k]);
                possibleSwapCom.push_back(temp1);
            }
        }
    }
    //check
    for(int i=0;i<possibleSwapCom.size();i++){
        for(int j=0;j<possibleSwapCom[i].size();j++){
            cout<<"["<<possibleSwapCom[i][j][0]<<" "<<possibleSwapCom[i][j][1]<<"] ";
        }
        cout<<endl;
    }
    int qubitNum = this->env->getQubitNum();
    vector<int> initialMapping;
    for(int i=0;i<qubitNum;i++){
        initialMapping.push_back(i);
    }
    vector<vector<int>> mappings;
    set<vector<int>> mappingSet;
    mappings.push_back(initialMapping);
    mappingSet.insert(initialMapping);
    int count=0;
    for(int i=0;i<klayer;i++){
        int mappingsNum=mappings.size();
        for(int j=0;j<mappingsNum;j++){
            for(int k=0;k<possibleSwapCom.size();k++){
                count++;
                vector<int> newMapping=mappings[j];
                for(int l=0;l<possibleSwapCom[k].size();l++){
                    int temp=newMapping[possibleSwapCom[k][l][0]];
                    newMapping[possibleSwapCom[k][l][0]]=newMapping[possibleSwapCom[k][l][1]];
                    newMapping[possibleSwapCom[k][l][1]]=temp;
                }
                if(mappingSet.count(newMapping)==0){
                    mappings.push_back(newMapping);
                    mappingSet.insert(newMapping);
                }
            }
        }
    }
//    cout<<"all the mapping : "<<count<<endl;
//    for(int aa=0;aa<mappings.size();aa++){
//        for(int bb=0;bb<mappings[aa].size();bb++){
//            cout<<mappings[aa][bb]<<" ";
//        }
//        cout<<endl;
//    }

    DefaultQueue queueForSwap;
    vector<int> qubitState(qubitNum,0);
    vector<ActionPath> initialPath;
    vector<vector<int>> dagTable=this->env->getGateDag();
    cout<<mappings.size()<<endl;
    for(int i=0;i<mappings.size();i++){
        SearchNode* sn=new SearchNode(mappings[i],qubitState,dagTable,this->env,0,initialPath);
        queueForSwap.push(sn);
    }
    cout<<queueForSwap.numPushed<<endl;
    cout<<queueForSwap.numFiltered<<endl;
    vector<vector<int>> goodMapping;
    for(int i=0;i<2;i++){
        SearchNode* a=queueForSwap.pop();
        cout<<"hi";
        for(int j=0;j<a->l2pMapping.size();j++){
            cout<<a->l2pMapping[j]<<" ";
        }
        cout<<a->cost1;
        cout<<endl;
        goodMapping.push_back(a->l2pMapping);
    }
    return goodMapping;
}

vector<vector<int>> Search::GoodInitialMapping(string type) {
    if (type == "swap search") {
        //this search 5 layer
        int kLayer=5;
        return this->SwapSearch(kLayer);
    }
    return this->SwapSearch(5);
}


SearchResult Search::SearchKLayer(SearchNode *sn) {
    DefaultQueue *nodeQueue = new DefaultQueue();
    nodeQueue->push(sn);
    DefaultExpander nodeExpander(this->env);
    vector<int> searchNum;
    while (nodeQueue->size() >= 0) {
        SearchNode *expandeNode;
        expandeNode = nodeQueue->pop();
        nodeExpander.expand(nodeQueue, expandeNode);
        searchNum.push_back(nodeExpander.expandeNum);
        if (nodeExpander.findBestNode == true) {
            break;
        }
    }
    SearchResult sr;
    sr.finalPath = nodeExpander.actionPath;
    sr.searchNodeNum = searchNum;
    vector<int> queueNum;
    queueNum.push_back(nodeQueue->numPushed);
    sr.queueNum = queueNum;
    return sr;
}

SearchResult Search::SearchKLayersWithInitialMapping(vector<int> initialMapping, int k) {
    vector<int> originMapping = initialMapping;
    int qubitNum = this->env->getQubitNum();
    vector<int> qubitState(qubitNum, 0);
    vector<int> nowMapping = initialMapping;
    vector<vector<int>> allDag = this->env->getGateDag();
    vector<int> topoGate=this->env->getTopoGate();
    vector<int> transGates;
    vector<int> kLayerSearchNum;
    vector<int> kLayerQueueNum;
    int patternNum = 0;
    int swapNum = 0;
    vector<ActionPath> finalActions;
    int count=0;
    while (allDag[0].size() > k) {
        cout<<"count is "<<count<<endl;
        count++;
        vector<vector<int>> k_dag = this->env->getNewKLayerDag(transGates, k);
        cout<<"the new k_dag is :\n";
        for (int i = 0; i < k_dag[0].size(); i++) {
            for (int j = 0; j < k_dag.size(); j++) {
                cout << k_dag[j][i] << " ";
            }
            cout << endl;
        }
        vector<ActionPath> initialActionPath;
        SearchNode *sn = new SearchNode(initialMapping, qubitState, k_dag, this->env, 0, initialActionPath);
        SearchResult srk = this->SearchKLayer(sn);
        kLayerSearchNum.push_back(srk.searchNodeNum[0]);
        kLayerQueueNum.push_back(srk.queueNum[0]);
        vector<int> originReadyGates = sn->readyGate;
        if (srk.finalPath[0].pattern == true) {
            patternNum++;
        }
        //new node
        for (int i = 0; i < srk.finalPath[0].actions.size(); i++) {
            if (srk.finalPath[0].actions[i].gateName == "swap") {
                int phyQubit1 = srk.finalPath[0].actions[i].targetQubit;
                int phyQubit2 = srk.finalPath[0].actions[i].controlQubit;
                int temp = initialMapping[phyQubit1];
                initialMapping[phyQubit1] = initialMapping[phyQubit2];
                initialMapping[phyQubit2] = temp;
                qubitState[initialMapping[phyQubit1]] = 2;
                qubitState[initialMapping[phyQubit2]] = 2;
                swapNum++;
            } else {
                transGates.push_back(srk.finalPath[0].actions[i].gateID);
                remove(topoGate.begin(), topoGate.end(), srk.finalPath[0].actions[i].gateID);
            }
        }
        finalActions.push_back(initialActionPath[0]);
        allDag=this->env->generateDag(topoGate);

    }
    vector<ActionPath> initialActionPath;
    SearchNode *sn = new SearchNode(initialMapping, qubitState, allDag, this->env, 0, initialActionPath);
    SearchResult srk = this->SearchKLayer(sn);
    kLayerSearchNum.push_back(srk.searchNodeNum[0]);
    kLayerQueueNum.push_back(srk.queueNum[0]);
    for (int j = 0; j < srk.finalPath.size(); j++) {
        finalActions.push_back(srk.finalPath[j]);
        if (srk.finalPath[j].pattern == true) {
            patternNum++;
        }
        for (int i = 0; i < srk.finalPath[j].actions.size(); i++) {
            if (srk.finalPath[j].actions[i].gateName == "swap") {
                int phyQubit1 = srk.finalPath[j].actions[i].targetQubit;
                int phyQubit2 = srk.finalPath[j].actions[i].controlQubit;
                int temp = initialMapping[phyQubit1];
                initialMapping[phyQubit1] = initialMapping[phyQubit2];
                initialMapping[phyQubit2] = temp;
                swapNum++;
            }
        }
    }
    SearchResult sr;
    sr.finalPath = finalActions;
    sr.initialMapping = originMapping;
    sr.finalMapping = initialMapping;
    sr.searchNodeNum = kLayerSearchNum;
    sr.queueNum = kLayerQueueNum;
    sr.patternNum = patternNum;
    sr.swapNum = swapNum;
    return sr;
}

vector<SearchResult *> Search::SearchPath(int k, string type) {
    vector<vector<int>> goodMapping = GoodInitialMapping(type);
    int qubitNum = this->env->getQubitNum();
    vector<int> qubitState(qubitNum, 0);
    vector<SearchResult *> searchResults;
    for (int i = 0; i < goodMapping.size(); i++) {
        vector<int> initialMapping = goodMapping[i];
        SearchResult sr = this->SearchKLayersWithInitialMapping(initialMapping, k);
        searchResults.push_back(&sr);
    }
    return searchResults;
}


SearchResult Search::SearchCircuit(SearchNode *sn) {
//    for(int i=0;i<sn->dagTable[0].size();i++){
//        for(int j=0;j<sn->dagTable.size();j++){
//            cout<<sn->dagTable[j][i]<<" ";
//        }
//        cout<<endl;
//    }
    DefaultQueue *nodeQueue = new DefaultQueue();
    nodeQueue->push(sn);
    DefaultExpander nodeExpander(this->env);
    vector<int> searchNum;
    int cycleNum=0;
    int whilecount=1;
    while (nodeQueue->size() >= 0) {
//        cout<<"while count : "<<whilecount<<endl;
        whilecount++;
//        cout<<"queue node size : "<<nodeQueue->size()<<endl;
        bool ifFind;
        SearchNode *expandeNode;
        expandeNode = nodeQueue->pop();
        if(whilecount%1000==0){
            cout<<"while count : "<<whilecount<<endl;
            cout<<"queue node size : "<<nodeQueue->size()<<endl;
            cout<<endl<<endl<<endl;
            cout<<"father node path length is : "<<expandeNode->actionPath.size()<<" and the timestamp is : "<<expandeNode->timeStamp<<endl;
            expandeNode->PrintNode();
        }
        ifFind=nodeExpander.expand1(nodeQueue, expandeNode);
        searchNum.push_back(nodeExpander.expandeNum);
        cycleNum=cycleNum+nodeExpander.cycleNum;
        if (ifFind == true) {
            break;
        }
    }
    SearchResult sr;
    sr.finalPath = nodeExpander.actionPath;
    sr.searchNodeNum = searchNum;
    vector<int> queueNum;
    queueNum.push_back(nodeQueue->numPushed);
    sr.queueNum = queueNum;
    sr.cycleNum=cycleNum;
    return sr;
}

SearchResult Search::SearchSmoothWithInitialMapping(vector<int> mapping, int k) {
    vector<int> originMapping = mapping;
    int qubitNum = this->env->getQubitNum();
    vector<int> qubitState(qubitNum, 0);
    vector<vector<int>> allDag = this->env->getGateDag();
    vector<int> topoGate=this->env->getTopoGate();
    int nowTime=0;
    vector<ActionPath> path;
    if(allDag[0].size()<=k){
        //如果层数小于k层，那么自己搜索完就好
        SearchNode *sn = new SearchNode(mapping, qubitState, allDag, env, nowTime, path);
        Search *sr = new Search(env);
        SearchResult a = this->SearchCircuit(sn);
        return a;
    }
    else{
        //如果层数大于k层，那么每次取前k层的dag
        SearchResult searR;
        searR.cycleNum=0;
        searR.patternNum=0;
        searR.swapNum=0;
        searR.initialMapping=mapping;
        vector<int> nowMapping=mapping;
        vector<int> nowQubitState(qubitNum, 0);
        vector<ActionPath>finalPath;
        while(topoGate.size()>0){
            vector<ActionPath> newPath;
            vector<vector<int>> kDag=env->getNewKLayerDag(topoGate,k);
            cout<<"topo gate : ";
            for(int i=0;i<topoGate.size();i++){
                cout<<topoGate[i]<<" ";
            }
            cout<<endl;
            cout<<"the k-dag depth is "<<kDag[0].size()<<endl;
            if(kDag[0].size()<=k){
                //如果最新的只有k层了，那么就直接搜索完
                SearchNode *sn = new SearchNode(nowMapping, nowQubitState, kDag, env, nowTime, newPath);
                Search *sr = new Search(env);
                SearchResult a = sr->SearchCircuit(sn);
                //把最后的每层的数据放到原来的final path里，统计计算swapNum的数目
                for(int i=0;i<a.finalPath.size();i++){
                    finalPath.push_back(a.finalPath[i]);
                    for(int j=0;j<a.finalPath[i].actions.size();j++){
                        if(a.finalPath[i].actions[j].gateName=="swap"){
                            searR.swapNum++;
                        }
                    }
                }
                //修改searchResult里面的统计数据
                int patternNum=0;
                for(int i=0;i<finalPath.size();i++){
                    if(finalPath[i].pattern==true){
                        patternNum++;
                    }
                }
                searR.finalPath=finalPath;
                searR.patternNum=patternNum;
                int searNum=0;
                for(int i=0;i<a.searchNodeNum.size();i++){
                    searNum=searNum+a.searchNodeNum[i];
                }
                searR.searchNodeNum.push_back(searNum);
                int queueNum=0;
                for(int i=0;i<a.queueNum.size();i++){
                    queueNum=queueNum+a.queueNum[i];
                }
                searR.queueNum.push_back(queueNum);
                searR.cycleNum=searR.cycleNum+a.cycleNum;
                return searR;
            }
            else{
                SearchNode *sn = new SearchNode(nowMapping, nowQubitState, kDag, env, nowTime, newPath);
                Search *sr = new Search(env);
                SearchResult a = sr->SearchCircuit(sn);
                //取完第一层后的结点状态
                finalPath.push_back(a.finalPath[0]);
                int swapNum;
                for(int i=0;i<a.finalPath[0].actions.size();i++){
                    if (a.finalPath[0].actions[i].gateName == "swap") {
                        //如果是swap修改mapping映射情况，以及qubitState
                        int phyQubit1 = a.finalPath[0].actions[i].targetQubit;
                        int phyQubit2 = a.finalPath[0].actions[i].controlQubit;
                        int temp = nowMapping[phyQubit1];
                        nowMapping[phyQubit1] = nowMapping[phyQubit2];
                        nowMapping[phyQubit2] = temp;
                        nowQubitState[nowMapping[phyQubit1]] = 3;
                        nowQubitState[nowMapping[phyQubit2]] = 3;
                        swapNum++;
                    }
                    else {
                        //如果是执行的结点，topoGate删除这个结点
                        int j=0;
                        for(j=0;j<topoGate.size();j++){
                            if(topoGate[j]==a.finalPath[0].actions[i].gateID){
                                break;
                            }
                        }
                        auto iter = topoGate.erase(topoGate.begin() + j);
                    }
                }
                //修改qubitState
                for(int i=0;i<nowQubitState.size();i++){
                    if(nowQubitState[i]>0){
                        nowQubitState[i]--;
                    }
                }
                nowTime++;
                //更新searchResult里面的数据
                int searNum=0;
                for(int i=0;i<a.searchNodeNum.size();i++){
                    searNum=searNum+a.searchNodeNum[i];
                }
                searR.searchNodeNum.push_back(searNum);
                int queueNum=0;
                for(int i=0;i<a.queueNum.size();i++){
                    queueNum=queueNum+a.queueNum[i];
                }
                searR.queueNum.push_back(queueNum);
                searR.cycleNum=searR.cycleNum+a.cycleNum;
                searR.swapNum=searR.swapNum+swapNum;
            }
        }
        return searR;
    }
}
