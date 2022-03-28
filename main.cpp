#include <iostream>
#include <map>
#include "src/parser/QASMparser.h"
#include "src/parser/Environment.h"
#include "src/search/SearchNode.h"
#include "src/expander/DefaultExpander.h"
#include "src/expander/Search.h"

using namespace std;

void PrintPath(SearchResult a) {
    cout << "how many path has done: " << a.finalPath.size() << endl;
    for (int i = 0; i < a.finalPath.size(); i++) {
        for (int j = 0; j < a.finalPath[i].actions.size(); j++) {
            cout << a.finalPath[i].actions[j].gateID << " " << a.finalPath[i].actions[j].gateName << " "
                 << a.finalPath[i].actions[j].controlQubit << " " << a.finalPath[i].actions[j].targetQubit << "   ";
        }
        cout << endl;
    }
    int count = 0;
    for (int i = 0; i < a.searchNodeNum.size(); i++) {
        count = count + a.searchNodeNum[i];
    }
    cout << "search node number: " << count << endl;
    int pattern = 0;
    for (int i = 0; i < a.finalPath.size(); i++) {
        if (a.finalPath[i].pattern == true) {
            pattern++;
        }
    }
    cout << "pattern number: " << pattern << endl;
    cout << "cycle num is " << a.cycleNum << endl;

}

int FindLastExcutedGate(SearchNode* sn,int logicalQubit){
    //找到当前状态sn中，logicalQubit上一个执行了的门，如果没有则返回0
    Environment* env=sn->environment;
    int lastGate=0;
    int firstGate=0;
    //找到接下来在logical qubit上要执行的下一个门，如果接下来没有待执行的门，那么=0
    for(int i=0;i<sn->dagTable[logicalQubit].size();i++){
        if(sn->dagTable[logicalQubit][i]!=0){
            firstGate=sn->dagTable[logicalQubit][i];
            break;
        }
    }
    int i;
    vector<vector<int>> allDagTable=env->getGateDag();
    for(i=allDagTable[logicalQubit].size()-1;i>=0;i--){
        if(allDagTable[logicalQubit][i]==firstGate){
            break;
        }
    }
    for(int j=i-1;j>=0;j--){
        if(allDagTable[logicalQubit][j]!=0){
            lastGate=allDagTable[logicalQubit][j];
            break;
        }
    }
    /*
//    auto iter = env->gateInfo.begin();
//    while(iter!=env->gateInfo.end()){
//        cout<<"gate id is : "<<iter->first<<" gate criticality is:"<<iter->second.criticality<<endl;
//        iter++;
//    }
//    //找到这个门的critiality，如果接下来没有待执行的门，那么=0
//    if(firstGate!=0){
//        criticality=env->gateInfo.find(firstGate)->second.criticality;
//    }
//    //从后往前找第一个不为0的门，如果到第一个也没有找到，那么说明这个logical qubit上没有已经执行了的门
//    vector<vector<int>> allDagTable=env->getGateDag();
//    for(int j=env->getDagDepth()-criticality-1;j>=0;j--){
//        if(allDagTable[logicalQubit][j]!=0){
//            lastGate=allDagTable[logicalQubit][j];
//            break;
//        }
//    }*/
    return lastGate;
}

int main() {

    string fname = "../circuits/small/test.qasm";
    vector<vector<int>> coupling_qx2,coupling_1x5;
    coupling_1x5={{0, 1},{1, 2},{2,3},{3,4}};
    Environment *env = new Environment(fname, coupling_1x5);
    auto iter = env->gateInfo.begin();
    cout<<"new info map is :"<<endl;
    while(iter!=env->gateInfo.end()){
        cout<<"gate id is : "<<iter->first<<" gate criticality is:"<<iter->second.criticality<<endl;
        iter++;
    }
    vector<int> mapping={0,1,2,3,4};
    vector<vector<int>> dagTable = env->getGateDag();
    cout<<"the all dag table:"<<endl;
    for(int i=0;i<dagTable[0].size();i++){
        for(int j=0;j<dagTable.size();j++){
            cout<<dagTable[j][i]<<" ";
        }
        cout<<endl;
    }
    vector<int> executedgateIDs={5,7};
    vector<vector<int>> dagTable5=env->getNewKLayerDag(executedgateIDs,15);
    cout<<"the new dag table:"<<endl;
    for(int i=0;i<dagTable5[0].size();i++){
        for(int j=0;j<dagTable5.size();j++){
            cout<<dagTable5[j][i]<<" ";
        }
        cout<<endl;
    }
    vector<int> qubitSate={0,0,0,0,0};
    vector<ActionPath> newPath;
    SearchNode *sn =new SearchNode(mapping,qubitSate,dagTable5, env, 1, newPath);
    int aa= FindLastExcutedGate(sn,4);
    cout<<"the last gate is:"<<aa<<endl;

//    string fname = "../circuits/small/test.qasm";
//    string fname1 = "../circuits/small/4gt11_84.qasm";
//    vector<vector<int>> coupling;
//    vector<vector<int>> coupling_qx2,coupling_1x5;
//    coupling_1x5={{0, 1},{1, 2},{2,3},{3,4}};
//    coupling_qx2={{0, 1},{1, 2},{2,3},{3,4},{0,2},{2,4}};
//    Environment *env = new Environment(fname1, coupling_1x5);
//    vector<int> mapping={0,1,2,3,4};
//    vector<vector<int>> dagTable = env->getGateDag();
//    vector<int> executedgateIDs={};
//    vector<vector<int>> dagTable5=env->getNewKLayerDag(executedgateIDs,15);
//    Search *sr = new Search(env);
//    vector<int> qubitSate={0,0,0,0,0};
//    vector<ActionPath> newPath;
//    SearchNode *sn =new SearchNode(mapping,qubitSate,dagTable5, env, 1, newPath);
////    sn->PrintNode();
//    SearchResult a = sr->SearchCircuit(sn);
//    PrintPath(a);


    return 0;
}
