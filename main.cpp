#include <iostream>
#include "src/parser/QASMparser.h"
#include "src/parser/environment.h"
using namespace std;

int main() {
    string fname="D:\\study\\StrongestMapper\\circuits\\mod5mils_65.qasm";
    vector<vector<int>> coupling;
    environment env(fname,coupling);
    return 0;
}
