//
// Created by wonder on 2021/12/20.
//

#ifndef STRONGESTMAPPER_DAG_H
#define STRONGESTMAPPER_DAG_H


#include "../parser/QASMparser.h"
//#include<list>
class Dag {
    struct DagNode{
        DagNode* controlParents;
        DagNode* targetParents;
        DagNode* controlChild;
        DagNode* targetChild;
        vector<DagNode*> rootChild;
        int gateId;//to denote the gate in GateNode
        int criticality;//length of circuit from here until furthest leaf
    };
public:
    Dag();
    Dag(vector<GateNode>);
    ~Dag();
    int* topologicalSort();
    int* frontLayer();
    Dag* firstKlayers(int K);
    Dag* deleteNodesWithID(int nodeID);

private:
    int gate_num;
    int qubit_num;
    int depth;
    DagNode* root;
};


#endif //STRONGESTMAPPER_DAG_H
