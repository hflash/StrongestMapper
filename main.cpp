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

int main() {
    string fname = "../circuits/small/test.qasm";
    string fname1 = "../circuits/small/4gt13_92.qasm";
    vector<vector<int>> coupling;
    vector<vector<int>> coupling_qx2,coupling_1x5;
    coupling_1x5={{0, 1},{1, 2},{2,3},{3,4}};
    coupling_qx2={{0, 1},{1, 2},{2,3},{3,4},{0,2},{2,4}};
    Environment *env = new Environment(fname, coupling_1x5);
    vector<int> mapping={0,1,2,3,4};
//    for(int i=0;i<env->getQubitNum();i++){
//        mapping.push_back(i);
//    }


    vector<vector<int>> dagTable = env->getGateDag();

    vector<int> executedgateIDs={};
    vector<vector<int>> dagTable5=env->getNewKLayerDag(executedgateIDs,8);
    Search *sr = new Search(env);
    vector<int> qubitSate={0,0,0,0,0};
    vector<ActionPath> newPath;
    SearchNode *sn =new SearchNode(mapping,qubitSate,dagTable5, env, 1, newPath);
//    sn->PrintNode();
    SearchResult a = sr->SearchCircuit1(sn);
    PrintPath(a);


//    for(int i=4;i<10;i++){
//        Search *sr = new Search(env);
//        SearchResult a = sr->SearchSmoothWithInitialMapping(mapping, i);
//        PrintPath(a);
//        cout<<"the k is :  "<<i<<"and the depth is : "<<a.finalPath.size()<<endl;
//    }

    return 0;
}
