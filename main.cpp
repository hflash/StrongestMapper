#include <iostream>
#include <map>
#include "src/parser/QASMparser.h"
#include "src/parser/Environment.h"
#include "src/slice/Slice.h"
#include "src/connect/Connect.h"
#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Eigenvalues>
#include <Python.h>

using namespace Eigen;
using namespace std;


int main() {
    string fname="../circuits/mod5mils_65.qasm";
    vector<vector<int>> coupling;
    coupling={{0,1},{1,2},{2,3},{3,4}};
    Environment env = Environment(fname,coupling);

//    //test of find parents and children of a gate by ID
//    for(int i = 0; i < env.getParentsByID(27).size();i++)
//        cout<<env.getParentsByID(27)[i]<<endl;
//    for(int i = 0; i < env.getParentsByID(27).size();i++)
//        cout<<env.getChildrenByID(27)[i]<<endl;
//    vector<int> exeGateIDs;
//    vector<vector<int>> kdag = env.getNewKLayerDag(exeGateIDs, 10);
//    vector<int> front = env.getFrontLayer(kdag);

//    //test of dag table and ID value  giving
//    map<int, GateNode> gateInfo = env.GetGateInfo();
//    map<int, GateNode>::iterator it;

//    for(it = gateInfo.begin();it != gateInfo.end();it++)
//    {
//        cout<<it->first;
//        cout<<it->second.Name;
//        cout<<it->second.criticality<<endl;
//    }
//    //test slice
//    Slice sli = Slice(env);
//    //constant slice
//    vector<vector<vector<int>>> slice_dag = sli.constant_slice(env, 10);
//    //adaptive slice
//    vector<vector<vector<int>>> slice_dag = sli.adaptive_slice(env, coupling);
//    for(int i=0; i < slice_dag.size(); i++){
//        cout<<"slice "<<i<<":"<<endl;
//        for(int j=0; j<slice_dag[i].size();j++){
//            for (int k = 0; k < slice_dag[i][j].size(); ++k) {
//                cout<<slice_dag[i][j][k]<<' ';
//            }
//            cout<<endl;
//        }
//    }

    //test python
    vector<vector<int>> cmap = {{0, 1}, {1, 2}, {3, 2}, {3, 8}, {4, 3}, {4, 9}, {5, 6}, {7, 6}, {7, 8}, {7, 8}, {9, 8}};
    vector<int> originLayout = {2, 3, 5, 7, 4, 9, 8, 0, 6, 1};
    vector<int> targetLayout = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    Connect con = Connect(cmap, originLayout, targetLayout);
    vector<vector<int>> swapSequance = con.swapSequence;
    for(int i=0; i<swapSequance.size(); i++){
        for (int j = 0; j < swapSequance[i].size(); ++j) {
            cout<<swapSequance[i][j]<<" ";
        }
        cout<<endl;
    }
    return 0;
}
