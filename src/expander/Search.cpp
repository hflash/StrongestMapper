//
// Created by mingz on 2022/1/7.
//

#include "Search.h"

Search::Search(Environment *env) {
    this->env = env;
}

vector<vector<int>> Search::SwapSearch(int klayer) {
    vector<vector<int>> possibleSwap = this->env->coupling;
//    cout<<"possible swap:\n";
//    for(int i=0;i<possibleSwap.size();i++){
//        cout<<"["<<possibleSwap[i][0]<<" "<<possibleSwap[i][1]<<"] ";
//    }
//    cout<<endl;
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
//                cout<<"before add:\n";
//                for(int aa=0;aa<temp1.size();aa++){
//                    cout<<"["<<temp1[aa][0]<<" "<<temp1[aa][1]<<"] ";
//                }
//                cout<<endl;
                temp1.push_back(possibleSwap[k]);
//                cout<<"after add:\n";
//                for(int aa=0;aa<temp1.size();aa++){
//                    cout<<"["<<temp1[aa][0]<<" "<<temp1[aa][1]<<"] ";
//                }
//                cout<<endl;
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
    cout<<"i am here\n";
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
                remove(topoGate.begin(), topoGate.end(), srk.finalPath[0].actions[i].gateID);
            }
        }
        finalActions.push_back(initialActionPath[0]);
        allDag=this->env->generateDag(topoGate);

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