#include <iostream>
#include <map>
#include "src/parser/QASMparser.h"
#include "src/parser/Environment.h"
using namespace std;

int main() {
    string fname="../circuits/mod5mils_65.qasm";
    vector<vector<int>> coupling;
    Environment env = Environment(fname,coupling);
    //test of find parents and children of a gate by ID
    for(int i = 0; i < env.getParentsByID(15).size();i++)
        cout<<env.getParentsByID(15)[i]<<endl;
    for(int i = 0; i < env.getParentsByID(15).size();i++)
        cout<<env.getChildrenByID(15)[i]<<endl;


    //test of dag table and ID value  giving
//    map<int, GateNode> gateInfo = env.GetGateInfo();
//    map<int, GateNode>::iterator it;

//    for(it = gateInfo.begin();it != gateInfo.end();it++)
//    {
//        cout<<it->first;
//        cout<<it->second.Name;
//        cout<<it->second.criticality<<endl;
//    }
    return 0;
}
