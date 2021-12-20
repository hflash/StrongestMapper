//
// Created by wonder on 2021/12/20.
//

#ifndef STRONGESTMAPPER_DAG_HPP
#define STRONGESTMAPPER_DAG_HPP


#include "../parser/QASMparser.h"
#include "DagNode.hpp"


class Dag{
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


#endif //STRONGESTMAPPER_DAG_HPP
