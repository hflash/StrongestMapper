//
// Created by mingz on 2022/1/7.
//

#include "Search.h"

Search::Search(Environment *env) {
    this->searchNum=0;
    this->env=env;
    this->patternNum=0;
    this->queueNum=0;
}

SearchResult Search::SearchKLayer(SearchNode* sn, vector<vector<int>> dagT) {
    DefaultQueue* nodeQueue=new DefaultQueue();
    nodeQueue->push(sn);
    DefaultExpander nodeExpander(this->env);
    while(nodeQueue->size()>=0){
        SearchNode* expandeNode;
        expandeNode=nodeQueue->pop();
        nodeExpander.expand(nodeQueue,expandeNode);
        if(nodeExpander.findBestNode==true){
            break;
        }
    }
    SearchResult sr;
    sr.finalPath=nodeExpander.actionPath;
    vector<int> searchNum;
    searchNum.push_back(nodeExpander.expandeNum);

    //这里queue的size是什么，是不是还没有统计queue里究竟放进去了多少个节点
    this->queueNum=this->queueNum+nodeQueue->size();
    return sr;
}

SearchResult Search::SearchKLayersWithInitialMapping(vector<int> initialMapping, int k) {
    vector<int> originMapping=initialMapping;
    int qubitNum=this->env->getQubitNum();
    vector<int> qubitState(qubitNum,0);
    vector<int> nowMapping=initialMapping;
    vector<vector<int>> allDag=this->env->getGateDag();
    vector<int> transGates;
    while(allDag.size()>k){
        vector<vector<int>> k_dag=this->env->getNewKLayerDag(transGates,k);
        vector<ActionPath> initialActionPath;
        SearchNode* sn=new SearchNode(initialMapping,qubitState,k_dag,this->env,0,initialActionPath);
        vector<ActionPath> nowActions=this->SearchKLayer(sn,k_dag);
        vector<int> originReadyGates=sn->readyGate;
        //new node
        for(int i=0;i<nowActions[0].actions.size();i++){
            if(nowActions[0].actions[i].gateName=="swap"){
                int phyQubit1=nowActions[0].actions[i].targetQubit;
                int phyQubit2=nowActions[0].actions[i].controlQubit;
                int temp=initialMapping[phyQubit1];
                initialMapping[phyQubit1]=initialMapping[phyQubit2];
                initialMapping[phyQubit2]=temp;
                qubitState[initialMapping[phyQubit1]]=2;
                qubitState[initialMapping[phyQubit2]]=2;
            }
            else{
                transGates.push_back(nowActions[0].actions[i].gateID);
                remove(allDag.begin(), allDag.end(), nowActions[0].actions[i].gateID);
            }
        }
        this->finalActions.push_back(initialActionPath[0]);
    }
    vector<ActionPath> initialActionPath;
    SearchNode* sn=new SearchNode(initialMapping,qubitState,allDag,this->env,0,initialActionPath);
    vector<ActionPath> nowActions=this->SearchKLayer(sn,allDag);
    for(int j=0;j<nowActions.size();j++){
        this->finalActions.push_back(nowActions[j]);
        for(int i=0;i<nowActions[0].actions.size();i++){
            if(nowActions[0].actions[i].gateName=="swap"){
                int phyQubit1=nowActions[0].actions[i].targetQubit;
                int phyQubit2=nowActions[0].actions[i].controlQubit;
                int temp=initialMapping[phyQubit1];
                initialMapping[phyQubit1]=initialMapping[phyQubit2];
                initialMapping[phyQubit2]=temp;
            }
        }
    }
    SearchResult sr;
    sr.finalPath=finalActions;
    sr.initialMapping=originMapping;
    sr.finalMapping=initialMapping;
}

void Search::SearchPath(int k) {
    vector<vector<int>> goodMapping=GoodInitialMapping();
    int qubitNum=this->env->getQubitNum();
    vector<int> qubitState(qubitNum,0);

    for(int i=0;i<goodMapping.size();i++){
        vector<int> initialMapping=goodMapping[i];
        vector<int> nowMapping=initialMapping;
        vector<vector<int>> allDag=this->env->getGateDag();
        vector<int> transGates;
        while(allDag.size()>k){
            vector<vector<int>> k_dag=this->env->getNewKLayerDag(transGates,k);
            vector<ActionPath> initialActionPath;
            SearchNode* sn=new SearchNode(initialMapping,qubitState,k_dag,this->env,0,initialActionPath);
            vector<ActionPath> nowActions=this->SearchKLayer(sn,k_dag);
            vector<int> originReadyGates=sn->readyGate;
            //new node
            for(int i=0;i<nowActions[0].actions.size();i++){
                if(nowActions[0].actions[i].gateName=="swap"){
                    int phyQubit1=nowActions[0].actions[i].targetQubit;
                    int phyQubit2=nowActions[0].actions[i].controlQubit;
                    int temp=initialMapping[phyQubit1];
                    initialMapping[phyQubit1]=initialMapping[phyQubit2];
                    initialMapping[phyQubit2]=temp;
                    qubitState[initialMapping[phyQubit1]]=2;
                    qubitState[initialMapping[phyQubit2]]=2;
                }
                else{
                    transGates.push_back(nowActions[0].actions[i].gateID);
                    remove(allDag.begin(), allDag.end(), nowActions[0].actions[i].gateID);
                }
            }
            this->finalActions.push_back(initialActionPath[0]);
        }
        vector<ActionPath> initialActionPath;
        SearchNode* sn=new SearchNode(initialMapping,qubitState,allDag,this->env,0,initialActionPath);
        vector<ActionPath> nowActions=this->SearchKLayer(sn,allDag);
        for(int j=0;j<nowActions.size();j++){
            this->finalActions.push_back(nowActions[j]);
        }
    }
}

