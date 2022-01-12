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
    int qubitNum = this->env->getQubitNum();
    vector<int> initialMapping;
    for(int i=0;i<qubitNum;i++){
        initialMapping.push_back(i);
    }
    vector<vector<int>> mappings;
    for(int i=0;i<klayer;i++){
        int mappingsNum=mappings.size();
        for(int j=0;j<mappingsNum;j++){
            for(int k=0;k<possibleSwapCom.size();k++){
                vector<int> newMapping=mappings[j];
                for(int l=0;l<possibleSwapCom[k].size();l++){
                    int temp=newMapping[possibleSwapCom[k][l][0]];
                    newMapping[possibleSwapCom[k][l][0]]=newMapping[possibleSwapCom[k][l][1]];
                    newMapping[possibleSwapCom[k][l][1]]=temp;
                }
                mappings.push_back(newMapping);
            }
        }
    }
    DefaultQueue queueForSwap;
    vector<int> qubitState(qubitNum,0);
    vector<ActionPath> initialPath;
    vector<vector<int>> dagTable=this->env->getGateDag();
    for(int i=0;i<mappings.size();i++){
        SearchNode* sn=new SearchNode(mappings[i],qubitState,dagTable,this->env,0,initialPath);
        queueForSwap.push(sn);
    }
    vector<vector<int>> goodMapping;
    for(int i=0;i<3;i++){
        goodMapping.push_back(queueForSwap.pop()->l2pMapping);
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

SearchResult Search::SearchKLayer(SearchNode *sn, vector<vector<int>> dagT) {
    DefaultQueue *nodeQueue = new DefaultQueue();
    nodeQueue->push(sn);
    DefaultExpander nodeExpander(this->env);
    while (nodeQueue->size() >= 0) {
        SearchNode *expandeNode;
        expandeNode = nodeQueue->pop();
        nodeExpander.expand(nodeQueue, expandeNode);
        if (nodeExpander.findBestNode == true) {
            break;
        }
    }
    SearchResult sr;
    sr.finalPath = nodeExpander.actionPath;
    vector<int> searchNum;
    searchNum.push_back(nodeExpander.expandeNum);
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
    vector<int> transGates;
    vector<int> kLayerSearchNum;
    vector<int> kLayerQueueNum;
    int patternNum = 0;
    int swapNum = 0;
    vector<ActionPath> finalActions;
    while (allDag.size() > k) {
        vector<vector<int>> k_dag = this->env->getNewKLayerDag(transGates, k);
        vector<ActionPath> initialActionPath;
        SearchNode *sn = new SearchNode(initialMapping, qubitState, k_dag, this->env, 0, initialActionPath);
        SearchResult srk = this->SearchKLayer(sn, k_dag);
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
                remove(allDag.begin(), allDag.end(), srk.finalPath[0].actions[i].gateID);
            }
        }
        finalActions.push_back(initialActionPath[0]);
    }
    vector<ActionPath> initialActionPath;
    SearchNode *sn = new SearchNode(initialMapping, qubitState, allDag, this->env, 0, initialActionPath);
    SearchResult srk = this->SearchKLayer(sn, allDag);
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