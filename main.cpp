#include <iostream>
#include <map>
#include "src/parser/QASMparser.h"
#include "src/parser/Environment.h"
#include "src/search/SearchNode.h"
#include "src/expander/DefaultExpander.h"
#include "src/expander/Search.h"

using namespace std;

void PrintPath(SearchResult a){
    cout << "how many path has done: " << a.finalPath.size() << endl;
    for (int i = 0; i < a.finalPath.size(); i++) {
        for (int j = 0; j < a.finalPath[i].actions.size(); j++) {
            cout << a.finalPath[i].actions[j].gateID << " " << a.finalPath[i].actions[j].gateName << " "
                 << a.finalPath[i].actions[j].controlQubit << " " << a.finalPath[i].actions[j].targetQubit << "   ";
        }
        cout << endl;
    }
    int count=0;
    for(int i=0;i<a.searchNodeNum.size();i++){
        count=count+a.searchNodeNum[i];
    }
    cout<<"search node number: "<<count<<endl;
    int pattern=0;
    for(int i=0;i<a.finalPath.size();i++){
        if(a.finalPath[i].pattern==true){
            pattern++;
        }
    }
    cout<<"pattern number: "<<pattern<<endl;
    cout<<"cycle num is "<<a.cycleNum<<endl;

}

int main() {

    string fname = "D:\\study\\SearchCompiler\\small\\small\\4mod5.qasm";
    vector<vector<int>> coupling;
    coupling = {{0, 1},
                {1, 2},
                {2, 3},
                {3, 4}};
    Environment *env = new Environment(fname, coupling);
    DefaultExpander *exp = new DefaultExpander(env);
    vector<int> mapping = {0,1,2,3,4};
    vector<int> qubitState = {0, 0, 0, 0, 0};
    vector<vector<int>> dagTable = env->getGateDag();
    cout<<"dag depth : "<<dagTable[0].size()<<endl;
    int nowTime = 0;
    vector<ActionPath> path;
    SearchNode *sn = new SearchNode(mapping, qubitState, dagTable, env, nowTime, path);
/*    vector<vector<vector<int>>> aa=exp->SwapCom1(qubitState,mapping,dagTable);
    for(int i=0;i<aa.size();i++){
        for(int j=0;j<aa[i].size();j++){
            cout<<"swap: "<<aa[i][j][0]<<" "<<aa[i][j][1]<<"  ";
        }
        cout<<endl;
    }*/
    Search *sr = new Search(env);
//    SearchResult a = sr->SearchCircuit(sn);
    SearchResult a=sr->SearchSmoothWithInitialMapping(mapping,5);
    PrintPath(a);
    cout << endl;
    return 0;
}
