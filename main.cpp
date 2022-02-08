#include <iostream>
#include <map>
#include "src/parser/QASMparser.h"
#include "src/parser/Environment.h"
#include "src/search/SearchNode.h"
#include "src/expander/DefaultExpander.h"
#include "src/expander/Search.h"

using namespace std;

inline std::size_t hashFunc3
        (SearchNode *node) {

    std::size_t hashResult = 0;
    int numQubits = node->environment->getQubitNum();
    int numRemainGates = node->remainGate.size();
    //conbine into hash: qubit mapping
    for (int x = 0; x < numQubits; x++)
        hash_combine(hashResult, node->l2pMapping[x]);

    //combine into hash: remainGate
    //remainGate is unique topological sort
    for (int i = 0; i < numRemainGates; i++) {
        hash_combine(hashResult, node->remainGate[i]);
    }

    return hashResult;
}

void PrintPath(SearchResult a){
    cout << "how many path has done: " << a.finalPath.size() << endl;
    for (int i = 0; i < a.finalPath.size(); i++) {
        for (int j = 0; j < a.finalPath[i].actions.size(); j++) {
            cout << a.finalPath[i].actions[j].gateID << " " << a.finalPath[i].actions[j].gateName << " "
                 << a.finalPath[i].actions[j].controlQubit << " " << a.finalPath[i].actions[j].targetQubit << "   ";
        }
        cout << endl;
    }
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
    vector<int> mapping = {0, 1, 2, 3, 4};
    vector<int> mapping1 = {0, 1, 2, 4, 3};
    vector<int> qubitState = {0, 0, 0, 0, 0};
    vector<vector<int>> dagTable = env->getGateDag();
    int nowTime = 0;
    vector<ActionPath> path;
    SearchNode *sn = new SearchNode(mapping, qubitState, dagTable, env, nowTime, path);
    std::size_t hashResult = hashFunc2(sn);
    Search *sr = new Search(env);
    SearchResult a = sr->SearchCircuit(sn);
    cout << endl;
    return 0;
}
