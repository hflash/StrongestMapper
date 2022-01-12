#include <iostream>
#include <map>
#include "src/parser/QASMparser.h"
#include "src/parser/Environment.h"
#include "src/search/SearchNode.h"
#include "src/expander/DefaultExpander.h"
#include "src/expander/Search.h"
using namespace std;

int main() {
    string fname="D:\\study\\SearchCompiler\\circuits\\mod5mils_65.qasm";
    vector<vector<int>> coupling;
    coupling={{0,2},{1,2},{2,3},{2,4}};
    Environment* env = new Environment(fname,coupling);
    Search* searchBestAction= new Search(env);
    int k=5;
    string type="swap search";
    vector<SearchResult*> srs;
    srs=searchBestAction->SearchPath(k,type);
    return 0;
}
