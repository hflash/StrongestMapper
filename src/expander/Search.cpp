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

vector<ActionPath> Search::SearchKLayer(vector<int> initialMapping, vector<int> qubitState, vector<vector<int>> dagT) {
    vector<ActionPath> initialActionPath;
    int timestamp=0;
    SearchNode* sn=new SearchNode(initialMapping,initialMapping,qubitState,dagT,this->env,timestamp,initialActionPath);
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
    this->searchNum=this->searchNum+nodeExpander.expandeNum;
    //这里queue的size是什么，是不是还没有统计queue里究竟放进去了多少个节点
    this->queueNum=this->queueNum+nodeQueue->size();
    return nodeExpander.actionPath;
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
        vector<ActionPath> initialActionPath;
        while(allDag.size()>k){
            vector<vector<int>> k_dag=this->env->getNewKLayerDag(transGates,k);
        }
    }
}

